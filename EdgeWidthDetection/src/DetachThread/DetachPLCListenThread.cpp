#include "DetachPLCListenThread.h"

#include "Modules.hpp"

DetachPLCListenThread::DetachPLCListenThread(QObject* parent)
	: QThread(parent), running(false) {

}

DetachPLCListenThread::~DetachPLCListenThread()
{
	stopThread();
	wait(); // 等待线程安全退出
}

void DetachPLCListenThread::startThread()
{
	running = true;
	if (!isRunning()) {
		start(); // 启动线程
	}
}

void DetachPLCListenThread::stopThread()
{
	running = false;
}

void DetachPLCListenThread::run()
{
	while (running)
	{
		QThread::sleep(1);
		readPLCInfo();
	}
}

void DetachPLCListenThread::readPLCInfo()
{
	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;

	//plcControllerScheduler->readUInt16RegisterAsync();
}