#pragma once

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
