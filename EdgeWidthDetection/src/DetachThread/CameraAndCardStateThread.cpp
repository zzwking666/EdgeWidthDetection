#include "CameraAndCardStateThread.h"
#include "Modules.hpp"
#include "Utilty.hpp"

size_t CameraAndCardStateThread::runtimeCounts = 0;

CameraAndCardStateThread::CameraAndCardStateThread(QObject* parent)
	: QThread(parent), running(false) {
}

CameraAndCardStateThread::~CameraAndCardStateThread()
{
	stopThread();
	wait();
}

void CameraAndCardStateThread::startThread()
{
	running = true;
	if (!isRunning()) {
		start();
	}
}

void CameraAndCardStateThread::stopThread()
{
	running = false;
}

void CameraAndCardStateThread::run()
{
	while (running) {
		QThread::msleep(2000);

		check_cameraState();

#ifndef BUILD_WITHOUT_HARDWARE
		// 无硬件模式下没有真实 PLC，跳过断连检测，避免误报与无效重连
		check_plcState();
#endif

		runtimeCounts++;
		if (runtimeCounts == 4) {
			runtimeCounts = 0;
		}
	}
}

void CameraAndCardStateThread::check_cameraState()
{
	check_cameraState1();
	check_cameraState2();
}

void CameraAndCardStateThread::check_cameraState1()
{
	auto& camera1 = Modules::getInstance().cameraModule.camera1;
	static bool isUpdateState = false;

	if (runtimeCounts != 0) {
		return;
	}

	if (camera1) {
		if (camera1->getConnectState()) {
			if (!isUpdateState) {
				emit updateCameraLabelState(1, true);
				isUpdateState = true;
			}
		}
		else {
			emit destroyCamera(1);
			emit updateCameraLabelState(1, false);
		}
	}
	else {
		emit buildCamera(1);
		emit startMonitor(1);
		emit updateCameraLabelState(1, false);
		isUpdateState = false;
	}
}

void CameraAndCardStateThread::check_cameraState2()
{
	auto& camera2 = Modules::getInstance().cameraModule.camera2;
	static bool isUpdateState = false;

	if (runtimeCounts != 0) {
		return;
	}

	if (camera2) {
		if (camera2->getConnectState()) {
			if (!isUpdateState) {
				emit updateCameraLabelState(2, true);
				isUpdateState = true;
			}
		}
		else {
			emit destroyCamera(2);
			emit updateCameraLabelState(2, false);
		}
	}
	else {
		emit buildCamera(2);
		emit startMonitor(2);
		emit updateCameraLabelState(2, false);
		isUpdateState = false;
	}
}

void CameraAndCardStateThread::check_plcState()
{
	auto& plcController = Modules::getInstance().plcController;
	static bool isUpdateState = false;

	// 每 2 秒探测一次（不随相机按 runtimeCounts 降频）：
	// 既保证断连能被及时发现，定期读寄存器也顺带充当心跳，避免 TCP 空闲被 PLC 主动 RST
	if (plcController.plcControllerScheduler) {
		if (probe_plcConnected()) {
			if (!isUpdateState) {
				emit updateCameraLabelState(0, true);
				isUpdateState = true;
			}
		}
		else {
			emit destroyPlc();
			emit updateCameraLabelState(0, false);
		}
	}
	else {
		emit buildPlc();
		emit updateCameraLabelState(0, false);
		isUpdateState = false;
	}
}

bool CameraAndCardStateThread::probe_plcConnected()
{
	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;

	if (!plcControllerScheduler) {
		return false;
	}

	// 实际读一次报警信息寄存器，以读写成败判断连接是否存活
	auto fut = plcControllerScheduler->readUInt16RegisterAsync(
		static_cast<uint16_t>(ModBusAddress::readPLCbaojingxinxiAddress));
	auto result = fut.get();
	return result.second;
}