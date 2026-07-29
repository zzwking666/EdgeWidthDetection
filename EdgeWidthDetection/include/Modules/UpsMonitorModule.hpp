#pragma once

#include <QAbstractNativeEventFilter>
#include <QObject>

#include "IModule.hpp"

/// UPS/电源状态监控模块
/// 监听 Windows 电源广播（WM_POWERBROADCAST），市电断开（切换到 UPS 电池供电）
/// 的瞬间发出信号，用于在系统关机流程开始之前就保存配置。
/// UPS 装好驱动后在 Windows 中表现为一块"电池"，本模块与具体 UPS 管理软件无关。
class UpsMonitorModule
	: public QObject, public IModule<bool>, public QAbstractNativeEventFilter
{
	Q_OBJECT
public:
	bool build() override;
	void destroy() override;
	void start() override;
	void stop() override;

	bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;

	/// 当前是否处于电池供电（市电断开）状态
	bool isOnBattery() const { return _onBattery; }
signals:
	/// 市电中断，已切换到 UPS 电池供电（batteryPercent 为电池剩余电量百分比，未知时为 255）
	void acPowerLost(int batteryPercent);
	/// 市电恢复
	void acPowerRestored();
private:
	/// 查询当前供电状态，状态变化时发出对应信号
	void checkPowerStatus();
private:
	bool _onBattery{ false };
	bool _filterInstalled{ false };
};
