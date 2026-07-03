#include "AutoExposureModule.hpp"

#include <algorithm>
#include <cmath>

#include "Modules.hpp"

bool AutoExposureModule::build()
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	_enabled = setConfig.autoExposureEnabled;
	_lastAdjustTime = std::chrono::steady_clock::now();
	return true;
}

void AutoExposureModule::destroy()
{
}

void AutoExposureModule::start()
{
}

void AutoExposureModule::stop()
{
}

void AutoExposureModule::setEnabled(bool enabled)
{
	_enabled = enabled;
}

void AutoExposureModule::onExposureStats(double meanIntensity, double overRatio, double underRatio)
{
	if (!_enabled) {
		return;
	}

	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	const auto now = std::chrono::steady_clock::now();
	const auto intervalMs = static_cast<long long>(setConfig.autoExposureAdjustIntervalMs);
	if (std::chrono::duration_cast<std::chrono::milliseconds>(now - _lastAdjustTime).count() < intervalMs) {
		return;
	}

	double error = 0.0;
	bool isMeanError = false;
	if (overRatio > setConfig.autoExposureMaxOverRatio) {
		error = -overRatio;
	} else if (underRatio > setConfig.autoExposureMaxUnderRatio) {
		error = underRatio;
	} else {
		error = (setConfig.autoExposureTargetMean - meanIntensity) / 255.0;
		isMeanError = true;
	}

	constexpr double kp = 0.01;
	constexpr double minStep = 30.0;          // 曝光调整步进，由 10 改为 30，降低灵敏度
	constexpr double maxStep = 500.0;
	constexpr double deadBand = 30.0;         // 死区与最小步进对齐，避免小范围来回跳动
	constexpr double errorThreshold = 0.02;   // 仅对均值误差生效，抑制轻微亮度波动

	if (isMeanError && std::abs(error) < errorThreshold) {
		return;
	}

	double currentExposure = 0.0;
	auto& cameraModule = Modules::getInstance().cameraModule;
	if (cameraModule.camera1) {
		currentExposure = static_cast<double>(cameraModule.camera1->getExposureTime());
	}
	if (currentExposure <= 0.0) {
		currentExposure = setConfig.autoExposureLastExposure > 0.0
			? setConfig.autoExposureLastExposure
			: setConfig.ruoguang;
	}

	double delta = currentExposure * kp * error;
	delta = std::clamp(delta, -maxStep, maxStep);
	if (std::abs(delta) < minStep) {
		delta = (delta >= 0.0) ? minStep : -minStep;
	}

	double newExposure = currentExposure + delta;
	newExposure = std::clamp(newExposure,
		setConfig.autoExposureMinExposure,
		setConfig.autoExposureMaxExposure);

	if (std::abs(newExposure - currentExposure) < deadBand) {
		return;
	}

	emit requestSetExposureTime(static_cast<size_t>(newExposure));
	setConfig.autoExposureLastExposure = newExposure;
	_lastAdjustTime = now;

	emit autoExposureInfoReady(newExposure, meanIntensity, overRatio, underRatio);
}
