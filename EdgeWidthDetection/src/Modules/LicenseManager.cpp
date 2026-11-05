#include "LicenseManager.hpp"

#include <QCryptographicHash>
#include <QDebug>
#include <QMessageAuthenticationCode>
#include <QSettings>

#include <algorithm>
#include <cctype>
#include <chrono>
#include <iterator>
#include <sstream>
#include <string>

#include "actCry_ActivationInfo.hpp"
#include "hoei_CPUInfo.hpp"
#include "hoei_MotherBoardInfo.hpp"
#include "LicensePolicy.hpp"
#include "Utilty.hpp"

namespace
{
	constexpr char kProductName[] = "EdgeWidthDetection";
	constexpr char kActivationKey[] = "EdgeWidthDetectionActivationKey";
	constexpr qint64 kPermanentThresholdMs = 4070908800000LL;	// 2099-01-01 UTC

	QString storageAppName()
	{
#ifdef BUILD_WITHOUT_HARDWARE
		return QStringLiteral("EdgeWidthDetectionLicenseTest");
#else
		return QStringLiteral("EdgeWidthDetectionLicense");
#endif
	}

	QSettings makeSettings()
	{
		return QSettings(QSettings::NativeFormat, QSettings::UserScope,
			QStringLiteral("RW"), storageAppName());
	}

	QByteArray protectionKey()
	{
		return QCryptographicHash::hash(
			QStringLiteral("EdgeWidthDetectionState|%1").arg(LicenseManager::getMachineCode()).toUtf8(),
			QCryptographicHash::Sha256);
	}

	QString valueMac(const QString& value)
	{
		return QString::fromLatin1(QMessageAuthenticationCode::hash(
			value.toUtf8(), protectionKey(), QCryptographicHash::Sha256).toHex());
	}

	bool loadProtectedValue(const QString& key, QString& value, bool& exists, QString& error)
	{
		QSettings settings = makeSettings();
		const QString macKey = key + QStringLiteral("Mac");
		const bool hasValue = settings.contains(key);
		const bool hasMac = settings.contains(macKey);
		if (!hasValue && !hasMac)
		{
			exists = false;
			return true;
		}
		if (!hasValue || !hasMac)
		{
			error = QStringLiteral("授权状态记录不完整");
			return false;
		}

		value = settings.value(key).toString();
		const QString actualMac = settings.value(macKey).toString();
		if (actualMac != valueMac(value))
		{
			error = QStringLiteral("授权状态记录校验失败");
			return false;
		}
		exists = true;
		return true;
	}

	bool saveProtectedValue(const QString& key, const QString& value, QString& error)
	{
		QSettings settings = makeSettings();
		settings.setValue(key, value);
		settings.setValue(key + QStringLiteral("Mac"), valueMac(value));
		settings.sync();
		if (settings.status() != QSettings::NoError)
		{
			error = QStringLiteral("授权状态保存失败，请检查系统注册表写入权限");
			return false;
		}
		return true;
	}

	QDateTime toUtcDateTime(const std::chrono::system_clock::time_point& timePoint)
	{
		const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
			timePoint.time_since_epoch()).count();
		return QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(ms), Qt::UTC);
	}

	QDateTime localMidnightUtc(const QDate& date)
	{
		return QDateTime(date, QTime(0, 0), Qt::LocalTime).toUTC();
	}

	bool isPermanent(const rw::actCry::ActivationInfo& info)
	{
		return toUtcDateTime(info.endTime).toMSecsSinceEpoch() >= kPermanentThresholdMs;
	}

	bool loadActivationInfo(rw::actCry::ActivationInfo& info, bool& exists, QString& error)
	{
		QString code;
		if (!loadProtectedValue(QStringLiteral("ActivationCode"), code, exists, error))
		{
			return false;
		}
		if (!exists)
		{
			return true;
		}

		bool parseOk = false;
		info = rw::actCry::ActivationInfo::parseActivationCode(
			code.toStdString(), kActivationKey, parseOk);
		if (!parseOk)
		{
			error = QStringLiteral("已保存的激活码无法解析");
			return false;
		}
		if (info.hwid != LicenseManager::getMachineCode().toStdString())
		{
			error = QStringLiteral("已保存的激活码与当前机器不匹配");
			return false;
		}
		return true;
	}

	QString policyFilePath()
	{
		QString path = globalPath.licensePolicyPath;
#ifdef BUILD_WITHOUT_HARDWARE
		if (qEnvironmentVariableIsSet("EWD_LICENSE_TEST_NOW"))
		{
			path += QStringLiteral(".test");
		}
#endif
		return path;
	}

	LicenseManager::Snapshot invalidSnapshot(const QString& reason)
	{
		LicenseManager::Snapshot snapshot;
		snapshot.state = LicenseManager::State::InvalidData;
		snapshot.statusText = QStringLiteral("授权状态异常：%1；当前为演示模式，禁止自动运行、PLC 写入和剔废输出")
			.arg(reason);
		return snapshot;
	}
}

QString LicenseManager::getMachineCode()
{
	// WMIC 等系统查询可能偶发失败；同一进程只计算一次，避免不同状态记录使用不同指纹
	static const QString machineCode = []() -> QString
	{
	auto toUpper = [](const std::string& str)
	{
		std::string result;
		result.reserve(str.size());
		for (unsigned char ch : str)
		{
			result.push_back(static_cast<char>(std::toupper(ch)));
		}
		return result;
	};

	auto isValidMotherboardUuid = [&toUpper](const std::string& uuid)
	{
		if (uuid.empty())
		{
			return false;
		}
		const std::string upper = toUpper(uuid);
		return upper != "UNKNOWN UUID"
			&& upper != "FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF"
			&& upper.find("O.E.M") == std::string::npos
			&& upper.find("TO BE FILLED") == std::string::npos;
	};

	auto getSystemDiskSerialNumber = []() -> std::string
	{
		char buffer[256];
		FILE* pipe = _popen("wmic path Win32_DiskDrive where Index=0 get SerialNumber", "r");
		if (!pipe)
		{
			return {};
		}

		std::string result;
		while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
		{
			result += buffer;
		}
		_pclose(pipe);

		std::istringstream stream(result);
		std::string line;
		std::getline(stream, line);
		std::getline(stream, line);
		line.erase(std::remove_if(line.begin(), line.end(), [](unsigned char ch) {
			return std::isspace(ch) || ch == '.';
		}), line.end());
		return line;
	};

	std::string mbUuid = rw::hoei::MotherBoardInfoFactory::GetMotherboardUniqueID();
	const std::string cpuModel = rw::hoei::CPUInfoFactory::GetCPUModel();
	const std::string diskSerial = getSystemDiskSerialNumber();
	if (!isValidMotherboardUuid(mbUuid))
	{
		mbUuid = "UNKNOWN_MB";
	}

	const std::string combined = std::string(kProductName) + "|" + mbUuid + "|" + cpuModel + "|" + diskSerial;
	const QByteArray data(combined.data(), static_cast<int>(combined.size()));
	return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
	}();
	return machineCode;
}

LicenseManager::Snapshot LicenseManager::evaluate(
	const QDateTime& effectiveNowUtc, bool clockRollback, bool stateError)
{
	QString error;
	rw::actCry::ActivationInfo activation;
	bool hasActivation = false;
	if (!loadActivationInfo(activation, hasActivation, error))
	{
		return invalidSnapshot(error);
	}

	Snapshot snapshot;
	if (hasActivation && isPermanent(activation))
	{
		snapshot.state = State::Permanent;
		snapshot.productionAllowed = true;
		snapshot.permanent = true;
		snapshot.expiresAt = toUtcDateTime(activation.endTime);
		snapshot.statusText = QStringLiteral("永久授权");
		return snapshot;
	}

	LicensePolicy::Data policy;
	if (!LicensePolicy::loadOrCreate(policyFilePath(), getMachineCode(), policy, error))
	{
		return invalidSnapshot(error);
	}

	const QDateTime firstDeadline = localMidnightUtc(policy.firstDeadline);
	const QDateTime secondDeadline = localMidnightUtc(policy.secondDeadline);
	const QDateTime thirdDeadline = localMidnightUtc(policy.thirdDeadline);

	if (hasActivation)
	{
		snapshot.expiresAt = toUtcDateTime(activation.endTime);
		const QDateTime startTime = toUtcDateTime(activation.startTime);
		if (effectiveNowUtc < startTime)
		{
			return invalidSnapshot(QStringLiteral("激活码尚未生效"));
		}
		if (snapshot.expiresAt == secondDeadline)
		{
			snapshot.state = State::Stage1;
		}
		else if (snapshot.expiresAt == thirdDeadline)
		{
			snapshot.state = State::Stage2;
		}
		else
		{
			snapshot.state = State::Stage1;
		}
	}
	else
	{
		snapshot.state = State::Trial;
		snapshot.expiresAt = firstDeadline;
	}

	if (stateError)
	{
		return invalidSnapshot(QStringLiteral("时间或授权状态记录损坏"));
	}
	if (clockRollback)
	{
		snapshot.state = State::ClockRollback;
		snapshot.productionAllowed = false;
		snapshot.statusText = QStringLiteral("检测到系统时间回拨；当前为演示模式，禁止自动运行、PLC 写入和剔废输出");
		return snapshot;
	}
	if (!snapshot.expiresAt.isValid() || effectiveNowUtc >= snapshot.expiresAt)
	{
		snapshot.state = State::Expired;
		snapshot.productionAllowed = false;
		snapshot.statusText = QStringLiteral("授权已于 %1 到期；当前为演示模式，禁止自动运行、PLC 写入和剔废输出")
			.arg(formatLocalTime(snapshot.expiresAt));
		return snapshot;
	}

	snapshot.productionAllowed = true;
	const QString expiry = formatLocalTime(snapshot.expiresAt);
	switch (snapshot.state)
	{
	case State::Trial:
		snapshot.statusText = QStringLiteral("试用授权有效，至 %1；到期后进入演示模式").arg(expiry);
		break;
	case State::Stage1:
		snapshot.statusText = QStringLiteral("第一阶段授权有效，至 %1").arg(expiry);
		break;
	case State::Stage2:
		snapshot.statusText = QStringLiteral("第二阶段授权有效，至 %1").arg(expiry);
		break;
	default:
		break;
	}
	return snapshot;
}

bool LicenseManager::applyActivationCode(
	const QString& activationCode, const QDateTime& effectiveNowUtc, QString& error)
{
	const QString code = activationCode.trimmed();
	if (code.isEmpty())
	{
		error = QStringLiteral("请输入激活码");
		return false;
	}

	bool parseOk = false;
	auto info = rw::actCry::ActivationInfo::parseActivationCode(code.toStdString(), kActivationKey, parseOk);
	if (!parseOk)
	{
		error = QStringLiteral("激活码无效或格式错误");
		return false;
	}
	if (info.hwid != getMachineCode().toStdString())
	{
		error = QStringLiteral("激活码与当前机器不匹配，请确认机器码后重新申请");
		return false;
	}
	if (effectiveNowUtc < toUtcDateTime(info.startTime))
	{
		error = QStringLiteral("激活码尚未生效");
		return false;
	}
	if (!isPermanent(info) && effectiveNowUtc >= toUtcDateTime(info.endTime))
	{
		error = QStringLiteral("激活码已过期，请申请当前阶段或永久激活码");
		return false;
	}

	rw::actCry::ActivationInfo oldInfo;
	bool hasOldInfo = false;
	QString loadError;
	if (loadActivationInfo(oldInfo, hasOldInfo, loadError) && hasOldInfo
		&& oldInfo.endTime > info.endTime)
	{
		error = QStringLiteral("新激活码的有效期短于当前授权，已拒绝覆盖");
		return false;
	}

	if (!saveProtectedValue(QStringLiteral("ActivationCode"), code, error))
	{
		return false;
	}
	return true;
}

bool LicenseManager::loadLastSeenTime(qint64& lastSeenMs, bool& exists, QString& error)
{
	QString value;
	if (!loadProtectedValue(QStringLiteral("LastSeenUtcMs"), value, exists, error))
	{
		return false;
	}
	if (!exists)
	{
		return true;
	}

	bool ok = false;
	lastSeenMs = value.toLongLong(&ok);
	if (!ok || lastSeenMs <= 0)
	{
		error = QStringLiteral("历史时间记录格式错误");
		return false;
	}
	return true;
}

bool LicenseManager::saveLastSeenTime(qint64 lastSeenMs, QString& error)
{
	return saveProtectedValue(QStringLiteral("LastSeenUtcMs"), QString::number(lastSeenMs), error);
}

QString LicenseManager::formatLocalTime(const QDateTime& utcTime)
{
	if (!utcTime.isValid())
	{
		return QStringLiteral("未知时间");
	}
	return utcTime.toLocalTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
}
