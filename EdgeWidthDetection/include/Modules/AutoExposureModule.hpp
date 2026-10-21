#pragma once

#include "IModule.hpp"

#include <QObject>
#include <chrono>

// 自动曝光硬性阈值：自动曝光设置的曝光值不允许超出 [kAutoExposureHardMin, kAutoExposureHardMax]
// 下限取 20：相机硬件最低支持约 11，留少量余量避免顶着硬件极限运行；
// 若过曝场景在 20 仍压不下来，应通过补光/增益解决而非继续下调
inline constexpr double kAutoExposureHardMin = 20.0;
inline constexpr double kAutoExposureHardMax = 1000.0;

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
	void autoExposureInfoReady(double currentExposure, double targetExposure, double meanIntensity,
		double overRatio, double underRatio);

private:
	void persistLastExposureThrottled(double newExposure, std::chrono::steady_clock::time_point now);

	bool _enabled{ false };
	size_t _cameraIndex{ 1 };
	std::chrono::steady_clock::time_point _lastAdjustTime;
	std::chrono::steady_clock::time_point _lastSaveTime;   // 默认零点，首次调节后立即落盘
	double _lastSavedExposure{ 0.0 };
};
