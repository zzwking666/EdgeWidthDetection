#include "LicenseModule.hpp"

#include <QDateTime>
#include <QDebug>

#include <algorithm>

namespace
{
	constexpr int kRefreshIntervalMs = 30000;
	constexpr qint64 kClockToleranceMs = 5 * 60 * 1000;
}

LicenseModule::LicenseModule()
{
	_refreshTimer.setInterval(kRefreshIntervalMs);
	connect(&_refreshTimer, &QTimer::timeout, this, &LicenseModule::refresh);
}

void LicenseModule::build()
{
	_wallAnchorMs = systemNowUtc().toMSecsSinceEpoch();
	_elapsedTimer.start();

	QString error;
	qint64 savedTime = 0;
	bool exists = false;
	if (!LicenseManager::loadLastSeenTime(savedTime, exists, error))
	{
		qWarning() << "授权时间状态读取失败:" << error;
		_stateError = true;
	}
	else if (exists)
	{
		_lastSeenMs.store(savedTime);
	}

	_built = true;
	persistLastSeenTime();
	refresh();
}

void LicenseModule::destroy()
{
	stop();
}

void LicenseModule::start()
{
	refresh();
	_refreshTimer.start();
}

void LicenseModule::stop()
{
	_refreshTimer.stop();
	if (_built)
	{
		persistLastSeenTime();
	}
}

bool LicenseModule::isProductionAllowed() const
{
	LicenseManager::Snapshot current = snapshot();
	if (!current.productionAllowed)
	{
		return false;
	}
	if (current.permanent)
	{
		return true;
	}
	return current.expiresAt.isValid()
		&& effectiveNowMs() < current.expiresAt.toMSecsSinceEpoch();
}

LicenseManager::Snapshot LicenseModule::snapshot() const
{
	std::lock_guard<std::mutex> lock(_snapshotMutex);
	return _snapshot;
}

bool LicenseModule::applyActivationCode(const QString& activationCode, QString& error)
{
	const QDateTime now = QDateTime::fromMSecsSinceEpoch(effectiveNowMs(), Qt::UTC);
	if (!LicenseManager::applyActivationCode(activationCode, now, error))
	{
		return false;
	}
	refresh();
	return true;
}

void LicenseModule::refresh()
{
	if (!_built)
	{
		return;
	}

	const qint64 systemMs = systemNowUtc().toMSecsSinceEpoch();
	const qint64 monotonicMs = monotonicNowMs();
	const qint64 lastSeenMs = _lastSeenMs.load();
	const qint64 referenceMs = std::max(monotonicMs, lastSeenMs);
	if (systemMs + kClockToleranceMs < referenceMs)
	{
		if (!_clockRollback)
		{
			qWarning() << "检测到系统时间回拨，禁止生产输出";
		}
		_clockRollback = true;
	}

	const qint64 effectiveMs = std::max(systemMs, referenceMs);
	_lastSeenMs.store(std::max(lastSeenMs, effectiveMs));
	persistLastSeenTime();

	LicenseManager::Snapshot next = LicenseManager::evaluate(
		QDateTime::fromMSecsSinceEpoch(effectiveMs, Qt::UTC), _clockRollback, _stateError);

	bool changed = false;
	{
		std::lock_guard<std::mutex> lock(_snapshotMutex);
		changed = next.state != _snapshot.state
			|| next.productionAllowed != _snapshot.productionAllowed
			|| next.expiresAt != _snapshot.expiresAt
			|| next.statusText != _snapshot.statusText;
		_snapshot = next;
	}
	if (changed)
	{
		emit stateChanged();
	}
}

QDateTime LicenseModule::systemNowUtc() const
{
#ifdef BUILD_WITHOUT_HARDWARE
	const QString testTime = qEnvironmentVariable("EWD_LICENSE_TEST_NOW");
	if (!testTime.isEmpty())
	{
		QDateTime parsed = QDateTime::fromString(testTime, Qt::ISODateWithMs);
		if (!parsed.isValid())
		{
			parsed = QDateTime::fromString(testTime, Qt::ISODate);
		}
		if (!parsed.isValid())
		{
			parsed = QDateTime::fromString(testTime, QStringLiteral("yyyy-MM-dd HH:mm:ss"));
			parsed.setTimeSpec(Qt::LocalTime);
		}
		if (parsed.isValid())
		{
			return parsed.toUTC();
		}
		qWarning() << "EWD_LICENSE_TEST_NOW 格式无效，应使用 ISO 日期时间或 yyyy-MM-dd HH:mm:ss";
	}
#endif
	return QDateTime::currentDateTimeUtc();
}

qint64 LicenseModule::effectiveNowMs() const
{
	return std::max({ systemNowUtc().toMSecsSinceEpoch(), monotonicNowMs(), _lastSeenMs.load() });
}

qint64 LicenseModule::monotonicNowMs() const
{
	if (!_elapsedTimer.isValid())
	{
		return systemNowUtc().toMSecsSinceEpoch();
	}
	return _wallAnchorMs + _elapsedTimer.elapsed();
}

void LicenseModule::persistLastSeenTime()
{
	if (_stateError)
	{
		return;
	}

	const qint64 current = effectiveNowMs();
	const qint64 previous = _lastSeenMs.load();
	const qint64 maximum = std::max(current, previous);
	_lastSeenMs.store(maximum);

	QString error;
	if (!LicenseManager::saveLastSeenTime(maximum, error))
	{
		qWarning() << "授权时间状态保存失败:" << error;
		_stateError = true;
	}
}
