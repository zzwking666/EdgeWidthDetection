#pragma once

#include <memory>

#include "hoem_ModbusDevice.hpp"
#include "hoem_ModbusDeviceScheduler.hpp"

class PlcController
{
public:
	std::shared_ptr<rw::hoem::ModbusDevice> plcController{};
	std::shared_ptr<rw::hoem::ModbusDeviceScheduler> plcControllerScheduler{};

	void build_plcController();
	void destroy_plcController();

private:
	bool loadConfigFromFile(std::string& device, int& baud);
	std::string getDesktopPath();
};
