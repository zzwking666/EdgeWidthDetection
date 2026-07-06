#pragma once

#include <QString>

/// 授权管理器：生成机器码、校验激活状态、应用激活码
namespace LicenseManager
{
	/// 生成当前机器的唯一机器码
	QString getMachineCode();

	/// 程序启动时的授权校验入口
	bool verifyAtStartup();

	/// 校验并保存用户输入的激活码
	bool applyActivationCode(const QString& activationCode);

	/// 当前是否已授权（从注册表读取并校验）
	bool isAuthorized();

	/// 获取授权状态字符串（当前实现为永久授权）
	QString getAuthorizationExpiry();
}
