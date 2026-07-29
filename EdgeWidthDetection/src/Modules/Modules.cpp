#include "Modules.hpp"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QMessageBox>

#include "DlgProductSet.h"
#include "EdgeWidthDetection.h"
#include "EdgeWidthDetection.hpp"
#include "lgm_PreDef.hpp"
#include "LicenseManager.hpp"
#include "rqw_RunEnvCheck.hpp"
#include "SetConfig.hpp"
#include "Utilty.hpp"


Modules::Modules()
{

}

Modules::~Modules()
{

}

bool Modules::build()
{
	// 读取配置
	auto configManagerBuild = configManagerModule.build();

	// 构建自动曝光模块
	autoExposureModule.setCameraIndex(1);
	auto autoExposureModuleBuild = autoExposureModule.build();
	autoExposureModule2.setCameraIndex(2);
	auto autoExposureModule2Build = autoExposureModule2.build();

	// 构建相机
	auto cameraBuild = cameraModule.build();

	// 构建显示信息模块
	auto runtimeInfoModuleBuild = runtimeInfoModule.build();

	// 构建图像处理模块
	auto imgProModuleBuild = imgProModule.build();

	// 构建UI模块
	uiModule.build();

	// 构建重连模块
	reconnectModule.build();

	// 构建消除模块
	auto eliminateModuleBuild = eliminateModule.build();

	// 构建图像保存模块
	imgSaveModule.build();

	// 构建PLC控制模块
	plcController.build();

#ifdef BUILD_WITHOUT_HARDWARE
	test_module.build();
#endif

	return true;
}

void Modules::destroy()
{
#ifdef BUILD_WITHOUT_HARDWARE
	test_module.destroy();
#endif
	plcController.destroy();
	runtimeInfoModule.destroy();
	imgProModule.destroy();
	cameraModule.destroy();
	configManagerModule.destroy();
	uiModule.destroy();
	reconnectModule.destroy();
	eliminateModule.destroy();
	imgSaveModule.destroy();
}

void Modules::start()
{
	uiModule.start();
	configManagerModule.start();
	runtimeInfoModule.start();
	imgSaveModule.start();
	eliminateModule.start();
	imgProModule.start();
	cameraModule.start();
	reconnectModule.start();
	plcController.start();
#ifdef BUILD_WITHOUT_HARDWARE
	test_module.start();
#endif
}

void Modules::stop()
{
#ifdef BUILD_WITHOUT_HARDWARE
	test_module.stop();
#endif
	plcController.stop();
	reconnectModule.stop();
	cameraModule.stop();
	imgProModule.stop();
	eliminateModule.stop();
	imgSaveModule.stop();
	runtimeInfoModule.stop();
	configManagerModule.stop();
	uiModule.stop();
}

void Modules::connect()
{
#pragma region connect camera and imgProModule
	QObject::connect(&cameraModule, &CameraModule::frameCaptured1,
		Modules::getInstance().imgProModule.imageProcessingModule1.get(), &ImageProcessingModule::onFrameCaptured, Qt::DirectConnection);

	QObject::connect(imgProModule.imageProcessingModule1.get(), &ImageProcessingModule::exposureStatsReady,
		&autoExposureModule, &AutoExposureModule::onExposureStats, Qt::QueuedConnection);

	QObject::connect(&autoExposureModule, &AutoExposureModule::requestSetExposureTime,
		&cameraModule, &CameraModule::onSetExposureTime, Qt::QueuedConnection);
#pragma endregion

#pragma region connect camera2 and imgProModule
	QObject::connect(&cameraModule, &CameraModule::frameCaptured2,
		Modules::getInstance().imgProModule.imageProcessingModule2.get(), &ImageProcessingModule::onFrameCaptured, Qt::DirectConnection);

	QObject::connect(imgProModule.imageProcessingModule2.get(), &ImageProcessingModule::exposureStatsReady,
		&autoExposureModule2, &AutoExposureModule::onExposureStats, Qt::QueuedConnection);

	QObject::connect(&autoExposureModule2, &AutoExposureModule::requestSetExposureTime,
		&cameraModule, &CameraModule::onSetExposureTime2, Qt::QueuedConnection);
#pragma endregion

#pragma region connect UIModule and ReconnectModule
	QObject::connect(reconnectModule.monitorCameraAndCardStateThread.get(), &CameraAndCardStateThread::updateCameraLabelState,
		uiModule._edgeWidthDetection, &EdgeWidthDetection::updateCameraLabelState);
#pragma endregion

#pragma region connect UIModule and imgProModule
	QObject::connect(imgProModule.imageProcessingModule1.get(), &ImageProcessingModule::imageReady,
		uiModule._edgeWidthDetection, &EdgeWidthDetection::onCameraDisplay);

	QObject::connect(imgProModule.imageProcessingModule2.get(), &ImageProcessingModule::imageReady,
		uiModule._edgeWidthDetection, &EdgeWidthDetection::onCameraDisplay);

	QObject::connect(uiModule._dlgProductSet,&DlgProductSet::paramsChanged,
		&imgProModule, &ImgProModule::onUpdateImgProContext);
#pragma endregion

#pragma region connect UIModules
	QObject::connect(uiModule._edgeWidthDetection, &EdgeWidthDetection::autoExposureToggled,
		&autoExposureModule, &AutoExposureModule::setEnabled);

	QObject::connect(uiModule._edgeWidthDetection, &EdgeWidthDetection::autoExposureToggled2,
		&autoExposureModule2, &AutoExposureModule::setEnabled);

	QObject::connect(&autoExposureModule, &AutoExposureModule::autoExposureInfoReady,
		uiModule._edgeWidthDetection, &EdgeWidthDetection::onAutoExposureInfo, Qt::QueuedConnection);

	QObject::connect(&autoExposureModule2, &AutoExposureModule::autoExposureInfoReady,
		uiModule._edgeWidthDetection, &EdgeWidthDetection::onAutoExposureInfo2, Qt::QueuedConnection);

	// UI 构造阶段信号槽尚未连接，此处根据配置显式同步自动曝光模块开关状态
	autoExposureModule.setEnabled(configManagerModule.setConfig.autoExposureEnabled1);
	autoExposureModule2.setEnabled(configManagerModule.setConfig.autoExposureEnabled2);
#pragma endregion

#pragma region connect camera and ReconnectModule
	// 相机重连
	QObject::connect(reconnectModule.monitorCameraAndCardStateThread.get(), &CameraAndCardStateThread::buildCamera,
		&cameraModule, &CameraModule::onBuildCamera);
	QObject::connect(reconnectModule.monitorCameraAndCardStateThread.get(), &CameraAndCardStateThread::startMonitor,
		&cameraModule, &CameraModule::onStartCamera);
	// 相机销毁
	QObject::connect(reconnectModule.monitorCameraAndCardStateThread.get(), &CameraAndCardStateThread::destroyCamera,
		&cameraModule, &CameraModule::onDestroyCamera);
#pragma endregion

#pragma region connect imgProModule and RuntimeInfoModule
	// 未使用DetachUtiltyThread
	/*QObject::connect(imgProModule.imageProcessingModule3.get(), &ImageProcessingPackageModule::updateStatisticalInfo,
		runtimeInfoModule.detachUtiltyThread.get(), &DetachUtiltyThread::updateStatisticalInfo);*/
#pragma endregion

#pragma region connect UIModule and RuntimeInfoModule
	QObject::connect(runtimeInfoModule.detachUtiltyThread.get(), &DetachUtiltyThread::updateStatisticalInfo,
		uiModule._edgeWidthDetection, &EdgeWidthDetection::onUpdateStatisticalInfoUI, Qt::QueuedConnection);
	// 原 label_warnningInfo 已改为显示自动曝光信息，PLC 报警信号不再连接到此标签
	/*QObject::connect(runtimeInfoModule.detachUtiltyThread.get(), &DetachUtiltyThread::updatePLCWarnningInfo,
		uiModule._edgeWidthDetection, &EdgeWidthDetection::onUpdatePLCWarnningInfoUI, Qt::QueuedConnection);*/
	QObject::connect(runtimeInfoModule.detachUtiltyThread.get(), &DetachUtiltyThread::updatePLCdaizishicechangduInfo,
		uiModule._edgeWidthDetection, &EdgeWidthDetection::onUpdatePLCdaizishicechangduInfoUI, Qt::QueuedConnection);
#pragma endregion

#pragma region connect UIModule and RuntimeInfoModule
	QObject::connect(plcController.plcListenThread.get(), &DetachPLCListenThread::updatePLCInfo,
		uiModule._dlgProductSet, &DlgProductSet::onUpdatePLCInfo);
#pragma endregion

#ifdef BUILD_WITHOUT_HARDWARE
	QObject::connect(test_module.testImgPushThread.get(), &TestImgPushThread::imgReady,
		imgProModule.imageProcessingModule1.get(), &ImageProcessingModule::onFrameCaptured, Qt::DirectConnection);
	QObject::connect(test_module.testImgPushThread.get(), &TestImgPushThread::imgReady2,
		imgProModule.imageProcessingModule2.get(), &ImageProcessingModule::onFrameCaptured, Qt::DirectConnection);
	#endif
}

bool Modules::check()
{
#pragma region check single instance
	if (!rw::rqw::RunEnvCheck::isSingleInstance("EdgeWidthDetection.exe"))
	{
		QMessageBox::warning(nullptr, "错误", "已经有程序在运行，请勿多次打开");
		return false;
	}
#pragma endregion

#pragma region check run env
	if (rw::rqw::RunEnvCheck::isProcessRunning("MVS.exe"))
	{
		QMessageBox::warning(nullptr, "错误", "检测到海康威视软件正在运行，请先关闭后再启动本程序。");
		return false;
	}

	if (rw::rqw::RunEnvCheck::isProcessRunning("BasedCam3.exe"))
	{
		QMessageBox::warning(nullptr, "错误", "检测到度申相机平台软件正在运行，请先关闭后再启动本程序。");
		return false;
	}
#pragma endregion

#pragma region check directory exist
	EnsureDirectoryExists(globalPath.projectHome);
	EnsureDirectoryExists(globalPath.configRootPath);
	EnsureDirectoryExists(globalPath.modelRootPath);
	EnsureDirectoryExists(globalPath.imageSaveRootPath);
	EnsureDirectoryExists(globalPath.upsRecordRootPath);
#pragma endregion

#pragma region check model exist
	if (!rw::rqw::RunEnvCheck::isFileExist(globalPath.modelPath))
	{
		QMessageBox::warning(nullptr, "错误", "模型文件缺失");
		return false;
	}
#pragma endregion

#pragma region check config format and exist
	rw::oso::StorageContext storageContext(rw::oso::StorageType::Xml);

	checkFileExistAndFormat<cdm::EdgeWidthDetectionConfig>(globalPath.EdgeWidthDetectionConfigPath, storageContext);
	checkFileExistAndFormat<cdm::SetConfig>(globalPath.setConfigPath, storageContext);
#pragma endregion

//#pragma region check license
//#ifndef BUILD_WITHOUT_HARDWARE
//	if (!LicenseManager::verifyAtStartup())
//	{
//		return false;
//	}
//#else
//	qDebug() << "[开发模式] 跳过机器码授权校验";
//#endif
//#pragma endregion

	return true;
}

void Modules::writeUpsRecord(const QString& content)
{
	// 确保目录存在（check() 已创建，这里兜底，因为关机回调可能发生在非常规路径）
	QDir().mkpath(globalPath.upsRecordRootPath);

	const QDateTime now = QDateTime::currentDateTime();
	const QString fileName = globalPath.upsRecordRootPath
		+ "UPS_" + now.toString("yyyyMMdd_HHmmss_zzz") + ".txt";

	QFile file(fileName);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		file.write(QString("[%1] %2\n")
			.arg(now.toString("yyyy-MM-dd HH:mm:ss.zzz"), content)
			.toUtf8());
		file.flush();
	}
}

bool Modules::EnsureDirectoryExists(const QString& dirPath)
{
	QDir dir(dirPath);
	if (!dir.exists()) {
		return dir.mkpath(".");
	}
	return true;
}
