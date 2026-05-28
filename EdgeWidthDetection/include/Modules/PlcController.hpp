#pragma once

#include <memory>
#include <QObject>

#include "DetachPLCListenThread.h"
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

	std::shared_ptr<DetachPLCListenThread> plcListenThread{};

	void build_plcListenThread();
	void destroy_plcListenThread();
private:
	bool loadConfigFromFile(std::string& ip, int& port);
	std::string getDesktopPath();
};
