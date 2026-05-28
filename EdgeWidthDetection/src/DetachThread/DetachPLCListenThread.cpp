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

	if (!plcControllerScheduler)
	{
		return;
	}

	QVector<PlcReadItem> datas{};

	// 实测宽度
	auto shicekuanduFUT = plcControllerScheduler->readUInt16RegisterAsync(
		ModBusAddress::shicekuanduAddress);

	auto ret = shicekuanduFUT.get(); // 只 get 一次
	datas.push_back(PlcReadItem{ ret.first, ret.second, 1 });

	// 设定标准值
	auto shedingbiaozhunzhiFUT = plcControllerScheduler->readUInt16RegisterAsync(
		ModBusAddress::shedingbiaozhunzhiAddress);

	ret = shedingbiaozhunzhiFUT.get(); // 只 get 一次
	datas.push_back(PlcReadItem{ ret.first, ret.second, 2 });

	// 长度间隔
	auto changdujiangeFUT = plcControllerScheduler->readUInt16RegisterAsync(
		ModBusAddress::changdujiangeAddress);

	ret = changdujiangeFUT.get(); // 只 get 一次
	datas.push_back(PlcReadItem{ ret.first, ret.second, 3 });

	// 拍照长度间隔
	auto paizhaochangdujiangeFUT = plcControllerScheduler->readUInt16RegisterAsync(
		ModBusAddress::paizhaochangdujiangeAddress);

	ret = paizhaochangdujiangeFUT.get(); // 只 get 一次
	datas.push_back(PlcReadItem{ ret.first, ret.second, 4 });

	// 步进一圈脉冲数
	auto bujinyiquanmaichongshuFUT = plcControllerScheduler->readUInt16RegisterAsync(
		ModBusAddress::bujinyiquanmaichongshuAddress);

	ret = bujinyiquanmaichongshuFUT.get(); // 只 get 一次
	datas.push_back(PlcReadItem{ ret.first, ret.second, 5 });

	// 螺距
	auto luojuFUT = plcControllerScheduler->readUInt16RegisterAsync(
		ModBusAddress::luojuAddress);

	ret = luojuFUT.get(); // 只 get 一次
	datas.push_back(PlcReadItem{ ret.first, ret.second, 6 });


	emit updatePLCInfo(datas);
}