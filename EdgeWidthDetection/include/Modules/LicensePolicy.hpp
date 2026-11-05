#pragma once

#include <QDate>
#include <QString>

namespace LicensePolicy
{
	struct Data
	{
		QDate firstDeadline{ 2026, 11, 6 };
		QDate secondDeadline{ 2026, 11, 7 };
		QDate thirdDeadline{ 2026, 11, 8 };
	};

	/// 读取并校验授权策略；文件被修改或机器码不匹配时返回 false
	bool load(const QString& path, const QString& machineCode, Data& outData, QString& error);

	/// 原子写入授权策略，并附带机器码绑定的完整性校验值
	bool save(const QString& path, const QString& machineCode, const Data& data, QString& error);

	/// 首次文件不存在时写入默认策略；已存在但损坏时不自动覆盖
	bool loadOrCreate(const QString& path, const QString& machineCode, Data& outData, QString& error);
}
