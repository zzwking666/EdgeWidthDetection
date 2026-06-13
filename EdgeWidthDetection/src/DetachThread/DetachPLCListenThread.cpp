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

	auto appendReadItem = [&](int address, int index)
		{
			auto fut = plcControllerScheduler->readUInt16RegisterAsync(address);
			auto ret = fut.get();
			datas.push_back(PlcReadItem{ ret.first, ret.second, index });
		};

	appendReadItem(ModBusAddress::shiceyahenkuanduAddress, PlcReadIndex::shiceyahenkuandu);
	appendReadItem(ModBusAddress::shedingyahenbiaozhunkuanduzhiAddress, PlcReadIndex::shedingyahenbiaozhunzhi);
	appendReadItem(ModBusAddress::paizhaojiangejuliAddress, PlcReadIndex::paizhaojiangejuli);
	appendReadItem(ModBusAddress::daizichangdubiaozhunAddress, PlcReadIndex::daizichangdubiaozhun);
	appendReadItem(ModBusAddress::jimiqiyiquanmaichongshuAddress, PlcReadIndex::jimiqiyiquanmaichongshu);
	appendReadItem(ModBusAddress::jimiqiyiquanzhouchangAddress, PlcReadIndex::jimiqiyiquanzhouchang);
	appendReadItem(ModBusAddress::bujinyiquanmaichongshuAddress, PlcReadIndex::bujinyiquanmaichongshu);
	appendReadItem(ModBusAddress::bujinluojuAddress, PlcReadIndex::bujinluoju);
	appendReadItem(ModBusAddress::daoyidongbuchangAddress, PlcReadIndex::daoyidongbuchang);
	appendReadItem(ModBusAddress::zidongdaoyidongzuidajuliAddress, PlcReadIndex::zidongdaoyidongzuidajuli);
	appendReadItem(ModBusAddress::daojiakeyidongdezuidajuliAddress, PlcReadIndex::daojiakeyidongdezuidajuli);
	appendReadItem(ModBusAddress::daoyidongsudumaichongpinlvAddress, PlcReadIndex::daoyidongsudumaichongpinlv);
	appendReadItem(ModBusAddress::shibiezhongxindianyutuxiangzhongxindianchazhiAddress, PlcReadIndex::shibiezhongxindianyutuxiangzhongxindianchazhi);

	appendReadItem(ModBusAddress::readPLCbaojingxinxiAddress, PlcReadIndex::readPLCbaojingxinxi);
	appendReadItem(ModBusAddress::readPLCkeyipaizhaoxinhaoAddress, PlcReadIndex::readPLCkeyipaizhaoxinhao);
	appendReadItem(ModBusAddress::readPLCtingzhipaizhaoxinhaoAddress, PlcReadIndex::readPLCtingzhipaizhaoxinhao);
	appendReadItem(ModBusAddress::readPLCdaizishicechangduAddress, PlcReadIndex::readPLCdaizishicechangdu);

	emit updatePLCInfo(datas);
}