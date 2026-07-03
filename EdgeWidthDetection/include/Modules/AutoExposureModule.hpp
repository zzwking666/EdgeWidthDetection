#pragma once

#include "IModule.hpp"

#include <QObject>
#include <chrono>

class AutoExposureModule : public QObject, public IModule<bool>
{
	Q_OBJECT
public:
	bool build() override;
	void destroy() override;
	void start() override;
	void stop() override;

public slots:
	void onExposureStats(double meanIntensity, double overRatio, double underRatio);
	void setEnabled(bool enabled);

signals:
	void requestSetExposureTime(size_t exposureTime);
	void autoExposureInfoReady(double targetExposure, double meanIntensity,
		double overRatio, double underRatio);

private:
	bool _enabled{ false };
	std::chrono::steady_clock::time_point _lastAdjustTime;
};
