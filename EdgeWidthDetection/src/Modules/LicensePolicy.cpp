#include "LicensePolicy.hpp"

#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageAuthenticationCode>
#include <QSaveFile>

namespace
{
	constexpr int kPolicyVersion = 1;

	bool isValid(const LicensePolicy::Data& data, QString& error)
	{
		if (!data.firstDeadline.isValid() || !data.secondDeadline.isValid() || !data.thirdDeadline.isValid())
		{
			error = QStringLiteral("授权节点日期无效");
			return false;
		}
		if (data.firstDeadline >= data.secondDeadline || data.secondDeadline >= data.thirdDeadline)
		{
			error = QStringLiteral("三个授权节点必须严格递增");
			return false;
		}
		return true;
	}

	QByteArray payload(const LicensePolicy::Data& data)
	{
		return QStringLiteral("%1|%2|%3|%4")
			.arg(kPolicyVersion)
			.arg(data.firstDeadline.toString(QStringLiteral("yyyy-MM-dd")))
			.arg(data.secondDeadline.toString(QStringLiteral("yyyy-MM-dd")))
			.arg(data.thirdDeadline.toString(QStringLiteral("yyyy-MM-dd")))
			.toUtf8();
	}

	QByteArray protectionKey(const QString& machineCode)
	{
		return QCryptographicHash::hash(
			QStringLiteral("EdgeWidthDetectionPolicy|%1").arg(machineCode).toUtf8(),
			QCryptographicHash::Sha256);
	}

	QByteArray mac(const LicensePolicy::Data& data, const QString& machineCode)
	{
		return QMessageAuthenticationCode::hash(payload(data), protectionKey(machineCode),
			QCryptographicHash::Sha256).toHex();
	}

	QDate parseDate(const QJsonObject& root, const QString& key, QString& error)
	{
		const QString text = root.value(key).toString();
		const QDate date = QDate::fromString(text, QStringLiteral("yyyy-MM-dd"));
		if (!date.isValid() || date.toString(QStringLiteral("yyyy-MM-dd")) != text)
		{
			error = QStringLiteral("授权节点 %1 格式错误，应为 yyyy-MM-dd").arg(key);
			return {};
		}
		return date;
	}
}

bool LicensePolicy::load(const QString& path, const QString& machineCode, Data& outData, QString& error)
{
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly))
	{
		error = QStringLiteral("无法读取授权策略文件：%1").arg(file.errorString());
		return false;
	}

	QJsonParseError parseError{};
	const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
	if (parseError.error != QJsonParseError::NoError || !document.isObject())
	{
		error = QStringLiteral("授权策略文件格式错误：%1").arg(parseError.errorString());
		return false;
	}

	const QJsonObject root = document.object();
	if (root.value(QStringLiteral("version")).toInt(-1) != kPolicyVersion)
	{
		error = QStringLiteral("授权策略版本不受支持");
		return false;
	}

	Data data;
	data.firstDeadline = parseDate(root, QStringLiteral("deadline1"), error);
	data.secondDeadline = parseDate(root, QStringLiteral("deadline2"), error);
	data.thirdDeadline = parseDate(root, QStringLiteral("deadline3"), error);
	if (!error.isEmpty() || !isValid(data, error))
	{
		return false;
	}

	const QByteArray expectedMac = mac(data, machineCode);
	const QByteArray actualMac = root.value(QStringLiteral("mac")).toString().toLatin1();
	if (actualMac.isEmpty() || actualMac != expectedMac)
	{
		error = QStringLiteral("授权策略校验失败，文件被修改或不属于当前机器");
		return false;
	}

	outData = data;
	return true;
}

bool LicensePolicy::save(const QString& path, const QString& machineCode, const Data& data, QString& error)
{
	if (machineCode.isEmpty())
	{
		error = QStringLiteral("机器码为空，无法生成授权策略");
		return false;
	}
	if (!isValid(data, error))
	{
		return false;
	}
	if (!QDir().mkpath(QFileInfo(path).absolutePath()))
	{
		error = QStringLiteral("无法创建授权策略目录");
		return false;
	}

	QJsonObject root;
	root.insert(QStringLiteral("version"), kPolicyVersion);
	root.insert(QStringLiteral("deadline1"), data.firstDeadline.toString(QStringLiteral("yyyy-MM-dd")));
	root.insert(QStringLiteral("deadline2"), data.secondDeadline.toString(QStringLiteral("yyyy-MM-dd")));
	root.insert(QStringLiteral("deadline3"), data.thirdDeadline.toString(QStringLiteral("yyyy-MM-dd")));
	root.insert(QStringLiteral("mac"), QString::fromLatin1(mac(data, machineCode)));

	QSaveFile file(path);
	if (!file.open(QIODevice::WriteOnly))
	{
		error = QStringLiteral("无法写入授权策略文件：%1").arg(file.errorString());
		return false;
	}
	if (file.write(QJsonDocument(root).toJson(QJsonDocument::Indented)) < 0 || !file.commit())
	{
		error = QStringLiteral("保存授权策略文件失败：%1").arg(file.errorString());
		return false;
	}
	return true;
}

bool LicensePolicy::loadOrCreate(const QString& path, const QString& machineCode, Data& outData, QString& error)
{
	if (!QFile::exists(path))
	{
		outData = Data{};
		return save(path, machineCode, outData, error);
	}
	return load(path, machineCode, outData, error);
}
