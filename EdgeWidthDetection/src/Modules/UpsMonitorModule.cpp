#include "UpsMonitorModule.hpp"

#include <QCoreApplication>
#include <windows.h>

#include "lgm_PreDef.hpp"

bool UpsMonitorModule::build()
{
	if (!_filterInstalled)
	{
		QCoreApplication::instance()->installNativeEventFilter(this);
		_filterInstalled = true;
	}

	// 启动时查询一次当前供电状态，仅记录不发射信号（此时信号槽尚未连接）
	SYSTEM_POWER_STATUS status{};
	if (GetSystemPowerStatus(&status))
	{
		_onBattery = (status.ACLineStatus == 0);
	}

	return true;
}

void UpsMonitorModule::destroy()
{
	if (_filterInstalled)
	{
		QCoreApplication::instance()->removeNativeEventFilter(this);
		_filterInstalled = false;
	}
}

void UpsMonitorModule::start()
{
}

void UpsMonitorModule::stop()
{
}

bool UpsMonitorModule::nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result)
{
	Q_UNUSED(result);
	if (eventType != "windows_generic_MSG" && eventType != "windows_dispatcher_MSG")
	{
		return false;
	}

	auto* msg = static_cast<MSG*>(message);
	if (msg->message != WM_POWERBROADCAST)
	{
		return false;
	}

	// PBT_APMPOWERSTATUSCHANGE：电源状态（市电/电池）发生变化
	// PBT_POWERSETTINGCHANGE：电源设置项变化（包含交直流电源切换 GUID_ACDC_POWER_SOURCE）
	if (msg->wParam == PBT_APMPOWERSTATUSCHANGE || msg->wParam == PBT_POWERSETTINGCHANGE)
	{
		checkPowerStatus();
	}

	// 不拦截消息，继续传递给其他处理者
	return false;
}

void UpsMonitorModule::checkPowerStatus()
{
	SYSTEM_POWER_STATUS status{};
	if (!GetSystemPowerStatus(&status))
	{
		return;
	}

	// ACLineStatus: 0=电池供电（市电断开）, 1=市电正常, 255=未知
	bool onBattery = (status.ACLineStatus == 0);
	if (onBattery == _onBattery)
	{
		return;
	}
	_onBattery = onBattery;

	if (_onBattery)
	{
		int batteryPercent = status.BatteryLifePercent;	// 0~100，未知为 255
		LOG_WARN("检测到市电中断，当前由 UPS 电池供电（剩余电量 {}%）", batteryPercent);
		LOG_FLUSH();
		emit acPowerLost(batteryPercent);
	}
	else
	{
		LOG_INFO("市电已恢复正常供电");
		LOG_FLUSH();
		emit acPowerRestored();
	}
}
