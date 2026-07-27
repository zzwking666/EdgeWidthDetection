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
	void setCameraIndex(size_t cameraIndex);

public slots:
	void onExposureStats(double meanIntensity, double overRatio, double underRatio);
	void setEnabled(bool enabled);

signals:
	void requestSetExposureTime(size_t exposureTime);
	void autoExposureInfoReady(double targetExposure, double meanIntensity,
		double overRatio, double underRatio);

private:
	void persistLastExposureThrottled(double newExposure, std::chrono::steady_clock::time_point now);

	bool _enabled{ false };
	size_t _cameraIndex{ 1 };
	std::chrono::steady_clock::time_point _lastAdjustTime;
	std::chrono::steady_clock::time_point _lastSaveTime;   // 默认零点，首次调节后立即落盘
	double _lastSavedExposure{ 0.0 };
};
