#pragma once

#include <atomic>
#include <memory>
#include <QObject>

// 实机未使用 PLC 轮询监听线程，先整体注释掉（不再创建该线程）
//#include "DetachPLCListenThread.h"
#include "hoem_ModbusDevice.hpp"
#include "hoem_ModbusDeviceScheduler.hpp"
#include "IModule.hpp"

class PlcController
	: public QObject, public IModule<void>
{
	Q_OBJECT
public:
	void build() override;
	void destroy() override;
	void start() override;
	void stop() override;
public:
	std::shared_ptr<rw::hoem::ModbusDevice> plcController{};
	std::shared_ptr<rw::hoem::ModbusDeviceScheduler> plcControllerScheduler{};

	// 手动写入总开关（由 DlgModbus 的「允许写入」复选框控制）：
	// 关闭后通讯界面与主界面面板的所有手动写入均被拦截并弹窗提示，所有读取不受影响
	std::atomic<bool> manualWriteEnabled{ true };

	void build_plcController();
	void destroy_plcController();

	// 实机未使用 PLC 轮询监听线程，先整体注释掉（不再创建该线程）
	//std::shared_ptr<DetachPLCListenThread> plcListenThread{};

	//void build_plcListenThread();
	//void destroy_plcListenThread();
public:
	bool getBuildResult() { return _buildResult; };
public slots:
	// 供断连监测线程（CameraAndCardStateThread）通过信号触发：先销毁再重建，完成重连
	void onBuildPlc();
	// 供断连监测线程通过信号触发：断开并释放 PLC 连接
	void onDestroyPlc();
private:
	bool loadConfigFromFile(std::string& ip, int& port);
	std::string getDesktopPath();

	bool _buildResult{ false };
};
