#include"DetachUtiltyThread.h"
#include "Modules.hpp"

DetachUtiltyThread::DetachUtiltyThread(QObject* parent)
	: QThread(parent), running(false) {
}

DetachUtiltyThread::~DetachUtiltyThread()
{
	stopThread();
	wait(); // 等待线程安全退出
}

void DetachUtiltyThread::startThread()
{
	running = true;
	if (!isRunning()) {
		start(); // 启动线程
	}
}

void DetachUtiltyThread::stopThread()
{
	running = false; // 停止线程
}

void DetachUtiltyThread::run()
{
	while (running) {
		QThread::sleep(1);
		emit updateStatisticalInfo();
		readPLCWarnningInfo();
	}
}

void DetachUtiltyThread::readPLCWarnningInfo()
{
	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;

	if (!plcControllerScheduler) {
		return;
	}

	if (0 == setConfig.registerAttribute)
	{
		auto fut = plcControllerScheduler->readUInt16RegisterAsync(
			static_cast<uint16_t>(ModBusAddress::readPLCbaojingxinxiAddress)
		);

		if (fut.get().second)
		{
			auto getResult = fut.get().first;
			emit updatePLCWarnningInfo(static_cast<uint16_t>(getResult));
		}
		else
		{
			qDebug() << "读取16位PLC报警信息失败";
		}
	}
	else if (1 == setConfig.registerAttribute)
	{
		auto fut = plcControllerScheduler->readUInt32RegisterAsync(
			static_cast<uint16_t>(ModBusAddress::readPLCbaojingxinxiAddress),
			rw::hoem::Endianness::BigEndian
		);

		if (fut.get().second)
		{
			auto getResult = fut.get().first;
			emit updatePLCWarnningInfo(static_cast<uint16_t>(getResult));
		}
		else
		{
			qDebug() << "读取32位大端PLC报警信息失败";
		}
	}
	else if (2 == setConfig.registerAttribute)
	{
		auto fut = plcControllerScheduler->readUInt32RegisterAsync(
			static_cast<uint16_t>(ModBusAddress::readPLCbaojingxinxiAddress),
			rw::hoem::Endianness::LittleEndian
		);

		if (fut.get().second)
		{
			auto getResult = fut.get().first;
			emit updatePLCWarnningInfo(static_cast<uint16_t>(getResult));
		}
		else
		{
			qDebug() << "读取32位小端PLC报警信息失败";
		}
	}
}
