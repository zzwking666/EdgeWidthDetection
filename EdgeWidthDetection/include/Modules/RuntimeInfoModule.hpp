#pragma once

#include"IModule.hpp"

#include<QObject>

#include "DetachUtiltyThread.h"
#include "Utilty.hpp"

class RuntimeInfoModule : public QObject, public IModule<bool>
{
	Q_OBJECT
public:
	bool build() override;
	void destroy() override;
	void start() override;
	void stop() override;
public:
	struct StatisticalInfo
	{
		// 仅运行模式（OpenRemoveFunc）统计，调试模式出图不计入
		std::atomic_uint64_t camera1PhotoCount{ 0 };		// 相机1拍照总量
		std::atomic_uint64_t camera1UnrecognizedCount{ 0 };	// 相机1未识别总量（本帧未向PLC写入有效值，写 0）
		std::atomic_uint64_t camera2PhotoCount{ 0 };		// 相机2拍照总量
		std::atomic_uint64_t camera2UnrecognizedCount{ 0 };	// 相机2未识别总量（本帧未向PLC写入有效值，写 0）
	} statisticalInfo;
public:
	std::atomic<RunningState> runningState{ RunningState::Stop };
public:
	std::unique_ptr<DetachUtiltyThread> detachUtiltyThread{ nullptr };
};

