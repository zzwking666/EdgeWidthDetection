#pragma once

#include <atomic>
#include <mutex>

#include <QElapsedTimer>
#include <QObject>
#include <QTimer>

#include "IModule.hpp"
#include "LicenseManager.hpp"

/// 授权运行模块：定时复检有效期、防系统时间回拨，并向生产输出提供线程安全门禁
class LicenseModule : public QObject, public IModule<void>
{
	Q_OBJECT
public:
	LicenseModule();

	void build() override;
	void destroy() override;
	void start() override;
	void stop() override;

	/// 图像处理/剔废线程可调用；除缓存有效外还会按单调时钟检查到期时间
	bool isProductionAllowed() const;
	LicenseManager::Snapshot snapshot() const;

	/// 保存激活码并立即复检；失败原因通过 error 返回
	bool applyActivationCode(const QString& activationCode, QString& error);

public slots:
	void refresh();

signals:
	void stateChanged();

private:
	QDateTime systemNowUtc() const;
	qint64 effectiveNowMs() const;
	qint64 monotonicNowMs() const;
	void persistLastSeenTime();

private:
	QTimer _refreshTimer;
	QElapsedTimer _elapsedTimer;
	qint64 _wallAnchorMs{ 0 };
	std::atomic<qint64> _lastSeenMs{ 0 };
	bool _clockRollback{ false };
	bool _stateError{ false };
	bool _built{ false };

	mutable std::mutex _snapshotMutex;
	LicenseManager::Snapshot _snapshot{};
};
