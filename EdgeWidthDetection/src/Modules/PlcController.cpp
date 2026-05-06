#include "PlcController.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <Windows.h>
#include <ShlObj.h>

void PlcController::build_plcController()
{
	rw::hoem::ModbusDeviceRtuCfg config;
	// 默认值
	config.device = "COM4";           // 使用 COM3 串口
	config.baud = 115200;               // 波特率 9600
	config.parity = 'N';              // 无校验
	config.dataBit = 8;               // 8 数据位
	config.stopBit = 1;               // 1 停止位
	config.baseAddress = 0;           // 无地址偏移

	// 尝试从配置文件加载参数
	std::string device;
	int baud;
	if (loadConfigFromFile(device, baud))
	{
		config.device = device;
		config.baud = baud;
		std::cout << "从配置文件加载参数成功: 串口=" << device << ", 波特率=" << baud << std::endl;
	}
	else
	{
		std::cout << "使用默认配置参数" << std::endl;
	}

	plcController = std::make_shared<rw::hoem::ModbusDevice>(config);

	bool isConnect = plcController->connect();

	if (isConnect)
	{
		plcControllerScheduler = std::make_shared<rw::hoem::ModbusDeviceScheduler>(plcController);
		std::cout << "PLC connected successfully." << std::endl;
	}
}

void PlcController::destroy_plcController()
{
	if (plcController)
	{
		plcController.reset();
	}
}

bool PlcController::loadConfigFromFile(std::string& device, int& baud)
{
	std::string configPath = getDesktopPath();
	if (configPath.empty())
	{
		std::cerr << "无法获取桌面路径" << std::endl;
		return false;
	}

	std::ifstream configFile(configPath);
	if (!configFile.is_open())
	{
		std::cerr << "无法打开配置文件: " << configPath << std::endl;
		return false;
	}

	std::string line;
	bool deviceFound = false;
	bool baudFound = false;

	while (std::getline(configFile, line))
	{
		// 移除行尾的逗号和空白字符
		line.erase(line.find_last_not_of(" \t\n\r\f\v,") + 1);

		if (line.find("串口名:") != std::string::npos)
		{
			size_t pos = line.find(":");
			if (pos != std::string::npos)
			{
				device = line.substr(pos + 1);
				// 移除前导空白字符
				device.erase(0, device.find_first_not_of(" \t"));
				deviceFound = true;
			}
		}
		else if (line.find("波特率:") != std::string::npos)
		{
			size_t pos = line.find(":");
			if (pos != std::string::npos)
			{
				std::string baudStr = line.substr(pos + 1);
				// 移除前导空白字符
				baudStr.erase(0, baudStr.find_first_not_of(" \t"));
				try
				{
					baud = std::stoi(baudStr);
					baudFound = true;
				}
				catch (const std::exception& e)
				{
					std::cerr << "波特率格式错误: " << e.what() << std::endl;
					return false;
				}
			}
		}
	}

	configFile.close();

	if (!deviceFound || !baudFound)
	{
		std::cerr << "配置文件缺少必要参数" << std::endl;
		return false;
	}

	return true;
}

std::string PlcController::getDesktopPath()
{
	WCHAR path[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_DESKTOP, NULL, 0, path)))
	{
		std::wstring ws(path);
		std::string desktopPath(ws.begin(), ws.end());
		return desktopPath + "\\config.txt";
	}
	return "";
}
