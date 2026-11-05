#pragma once

#include <QDateTime>
#include <QString>

namespace LicenseManager
{
	enum class State
	{
		Trial,
		Stage1,
		Stage2,
		Permanent,
		Expired,
		ClockRollback,
		InvalidData
	};

	struct Snapshot
	{
		State state{ State::InvalidData };
		bool productionAllowed{ false };
		bool permanent{ false };
		QDateTime expiresAt{};	// UTC
		QString statusText{};
	};

	/// 生成当前机器的唯一机器码
	QString getMachineCode();

	/// 根据有效时间、激活码和防回拨状态计算授权快照
	Snapshot evaluate(const QDateTime& effectiveNowUtc, bool clockRollback, bool stateError);

	/// 校验并保存机器码绑定的激活码，不向其他函数弹窗
	bool applyActivationCode(const QString& activationCode, const QDateTime& effectiveNowUtc, QString& error);

	/// 读取/保存带完整性校验的历史最大可信时间（UTC 毫秒）
	bool loadLastSeenTime(qint64& lastSeenMs, bool& exists, QString& error);
	bool saveLastSeenTime(qint64 lastSeenMs, QString& error);

	/// 供界面显示 UTC 时间对应的本地时间
	QString formatLocalTime(const QDateTime& utcTime);
}
