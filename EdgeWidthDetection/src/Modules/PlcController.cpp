#include "PlcController.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <Windows.h>
#include <ShlObj.h>

void PlcController::build()
{
	build_plcController();
	// 实机未使用 PLC 轮询监听线程，先整体注释掉（不再创建该线程）
	//build_plcListenThread();
}

void PlcController::destroy()
{
	//destroy_plcListenThread();
	destroy_plcController();
}

void PlcController::start()
{
	// 实机未使用 PLC 轮询监听线程，先整体注释掉（不再创建该线程）
	//if (plcListenThread)
	//{
	//	plcListenThread->startThread();
	//}
}

void PlcController::stop()
{
	//if (plcListenThread)
	//{
	//	plcListenThread->stopThread();
	//}
}

void PlcController::build_plcController()
{
	rw::hoem::ModbusDeviceTcpCfg config;
	// 默认值
	config.ip = "192.168.0.11";       // ip地址
	config.port = 502;                // Modbus TCP默认端口
	config.baseAddress = 0;           // 无地址偏移

	// 尝试从配置文件加载参数
	std::string ip;
	int port;
	if (loadConfigFromFile(ip, port))
	{
		config.ip = ip;
		config.port = port;
		std::cout << "从配置文件加载参数成功: ip=" << ip << ", 端口=" << port << std::endl;
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
		_buildResult = true;
	}
	else
	{
		// 连接失败时不创建调度器，由断连监测线程后续触发 onBuildPlc 重试
		plcController.reset();
		_buildResult = false;
		std::cout << "PLC connect failed." << std::endl;
	}
}

void PlcController::destroy_plcController()
{
	if (plcControllerScheduler)
	{
		plcControllerScheduler.reset();
	}
	if (plcController)
	{
		plcController.reset();
	}
}

// 实机未使用 PLC 轮询监听线程，先整体注释掉（不再创建该线程）
//void PlcController::build_plcListenThread()
//{
//	plcListenThread = std::make_shared<DetachPLCListenThread>();
//}
//
//void PlcController::destroy_plcListenThread()
//{
//	if (plcListenThread)
//	{
//		plcListenThread.reset();
//	}
//}

void PlcController::onBuildPlc()
{
	// 断连监测触发的重建：先释放旧连接再重新建立
	destroy_plcController();
	build_plcController();
}

void PlcController::onDestroyPlc()
{
	destroy_plcController();
}

bool PlcController::loadConfigFromFile(std::string& ip, int& port)
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
	bool ipFound = false;
	bool portFound = false;

	while (std::getline(configFile, line))
	{
		// 移除行尾的逗号和空白字符
		line.erase(line.find_last_not_of(" \t\n\r\f\v,") + 1);

		if (line.find("ip:") != std::string::npos)
		{
			size_t pos = line.find(":");
			if (pos != std::string::npos)
			{
				ip = line.substr(pos + 1);
				// 移除前导空白字符
				ip.erase(0, ip.find_first_not_of(" \t"));
				ipFound = true;
			}
		}
		else if (line.find("port:") != std::string::npos)
		{
			size_t pos = line.find(":");
			if (pos != std::string::npos)
			{
				std::string portStr = line.substr(pos + 1);
				// 移除前导空白字符
				portStr.erase(0, portStr.find_first_not_of(" \t"));
				try
				{
					port = std::stoi(portStr);
					portFound = true;
				}
				catch (const std::exception& e)
				{
					std::cerr << "端口格式错误: " << e.what() << std::endl;
					return false;
				}
			}
		}
	}

	configFile.close();

	if (!ipFound || !portFound)
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
