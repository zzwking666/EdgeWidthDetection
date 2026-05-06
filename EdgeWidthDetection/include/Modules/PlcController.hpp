#pragma once

#include <memory>
#include <QObject>

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

private:
	bool loadConfigFromFile(std::string& device, int& baud);
	std::string getDesktopPath();
};
