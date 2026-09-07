#include "EdgeWidthDetection.h"

#include <QDir>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QProcess>

#include "ui_EdgeWidthDetection.h"
#include <QPushButton>
#include <QButtonGroup>
#include <fmt/ranges.h>

#include "DlgLicense.h"
#include "DlgModbus.h"
#include "DlgProductSet.h"
#include "LicenseManager.hpp"
#include "Modules.hpp"
#include "NumberKeyboard.h"
#include "rqw_RunEnvCheck.hpp"
#include "Utilty.hpp"
#include "VersionInfo.hpp"


EdgeWidthDetection::EdgeWidthDetection(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::EdgeWidthDetectionClass())
{
	ui->setupUi(this);

	initializeComponents();
}

EdgeWidthDetection::~EdgeWidthDetection()
{
	Modules::getInstance().stop();
	Modules::getInstance().destroy();
	delete ui;
}

#ifdef BUILD_WITHOUT_HARDWARE
void EdgeWidthDetection::cBox_testPushImg_checked(bool checked)
{
	if (checked)
	{
		Modules::getInstance().test_module.testImgPush = true;
	}
	else
	{
		Modules::getInstance().test_module.testImgPush = false;
	}
}
#endif

void EdgeWidthDetection::build_ui()
{
	build_EdgeWidthDetectionData();
	build_DlgCloseForm();

#ifdef BUILD_WITHOUT_HARDWARE
	cBox_testPushImg = new QCheckBox(this);
	cBox_testPushImg->setText("图像推送状态");
	ui->gBox_infor->layout()->addWidget(cBox_testPushImg);
	QObject::connect(cBox_testPushImg, &QCheckBox::clicked,
		this, &EdgeWidthDetection::cBox_testPushImg_checked);
#endif
}

void EdgeWidthDetection::build_connect()
{
	QObject::connect(ui->pbtn_exit, &QPushButton::clicked,
		this, &EdgeWidthDetection::pbtn_exit_clicked);
	QObject::connect(ui->pbtn_set, &QPushButton::clicked,
		this, &EdgeWidthDetection::pbtn_set_clicked);
	QObject::connect(ui->pbtn_Modbus, &QPushButton::clicked,
		this, &EdgeWidthDetection::pbtn_Modbus_clicked);
	QObject::connect(ui->rbtn_debug, &QRadioButton::clicked,
		this, &EdgeWidthDetection::rbtn_debug_checked);
	QObject::connect(ui->rbtn_removeFunc, &QRadioButton::clicked,
		this, &EdgeWidthDetection::rbtn_removeFunc_checked);
	QObject::connect(ui->pbtn_resetProduct, &QPushButton::clicked,
		this, &EdgeWidthDetection::pbtn_resetProduct_clicked);
	QObject::connect(ui->pbtn_openSaveLocation, &QPushButton::clicked,
		this, &EdgeWidthDetection::pbtn_openSaveLocation_clicked);
	QObject::connect(ui->ckb_saveImg, &QCheckBox::clicked,
		this, &EdgeWidthDetection::ckb_saveImg_checked);
	QObject::connect(ui->ckb_autoExposure, &QCheckBox::clicked,
		this, &EdgeWidthDetection::ckb_autoExposure_checked);
	QObject::connect(ui->rbtn_ruoguang, &QRadioButton::clicked,
		this, &EdgeWidthDetection::rbtn_ruoguang_checked);
	QObject::connect(ui->rbtn_zhongguang, &QRadioButton::clicked,
		this, &EdgeWidthDetection::rbtn_zhongguang_checked);
	QObject::connect(ui->rbtn_qiangguang, &QRadioButton::clicked,
		this, &EdgeWidthDetection::rbtn_qiangguang_checked);
	QObject::connect(ui->rbtn_ruoguang_2, &QRadioButton::clicked,
		this, &EdgeWidthDetection::rbtn_ruoguang_2_checked);
	QObject::connect(ui->rbtn_zhongguang_2, &QRadioButton::clicked,
		this, &EdgeWidthDetection::rbtn_zhongguang_2_checked);
	QObject::connect(ui->rbtn_qiangguang_2, &QRadioButton::clicked,
		this, &EdgeWidthDetection::rbtn_qiangguang_2_checked);
	QObject::connect(ui->ckb_autoExposure_2, &QCheckBox::clicked,
		this, &EdgeWidthDetection::ckb_autoExposure_2_checked);
	// 连接显示标题
	QObject::connect(clickableTitle, &rw::rqw::ClickableLabel::clicked,
		this, &EdgeWidthDetection::lb_title_clicked);
}

void EdgeWidthDetection::build_EdgeWidthDetectionData()
{
	auto& maiLiDingZiConfig = Modules::getInstance().configManagerModule.edgeWidthDetectionConfig;
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	maiLiDingZiConfig.isDebug = false;
	maiLiDingZiConfig.isDefect = true;		// 默认开启剔废
	// 程序初始化时默认开启存图，用户仍可在界面上手动关闭
	maiLiDingZiConfig.isSaveImg = true;
	ui->ckb_saveImg->setChecked(true);

	ui->rbtn_removeFunc->setChecked(maiLiDingZiConfig.isDefect);

	rbtn_removeFunc_checked(true);

	// 初始化图像查看器
	_picturesViewer = new PictureViewerThumbnails(this);

	ini_clickableTitle();

	// button分类
	auto* groupA = new QButtonGroup(this); // 2个一组
	groupA->setExclusive(true);
	groupA->addButton(ui->rbtn_debug);
	groupA->addButton(ui->rbtn_removeFunc);

	auto* groupB = new QButtonGroup(this); // 4个一组：弱光/中光/强光/自动曝光
	groupB->setExclusive(true);
	groupB->addButton(ui->rbtn_ruoguang);
	groupB->addButton(ui->rbtn_zhongguang);
	groupB->addButton(ui->rbtn_qiangguang);
	groupB->addButton(ui->ckb_autoExposure);

	auto* groupB2 = new QButtonGroup(this); // 相机2：弱光/中光/强光/自动曝光
	groupB2->setExclusive(true);
	groupB2->addButton(ui->rbtn_ruoguang_2);
	groupB2->addButton(ui->rbtn_zhongguang_2);
	groupB2->addButton(ui->rbtn_qiangguang_2);
	groupB2->addButton(ui->ckb_autoExposure_2);

	// 恢复上次状态：自动曝光与手动档位互斥
	if (setConfig.autoExposureEnabled1)
	{
		ui->ckb_autoExposure->setChecked(true);
	}
	else
	{
		switch (setConfig.lastChooseLight1)
		{
		case 0:
			ui->rbtn_ruoguang->setChecked(true);
			rbtn_ruoguang_checked(true);
			break;
		case 1:
			ui->rbtn_zhongguang->setChecked(true);
			rbtn_zhongguang_checked(true);
			break;
		case 2:
			ui->rbtn_qiangguang->setChecked(true);
			rbtn_qiangguang_checked(true);
			break;
		default:
			ui->rbtn_ruoguang->setChecked(true);
			rbtn_ruoguang_checked(true);
			break;
		}
	}

	// 恢复相机2上次状态：自动曝光与手动档位互斥（相机2自动曝光信息与相机1共用 label_warnningInfo，独立分行显示）
	if (setConfig.autoExposureEnabled2)
	{
		ui->ckb_autoExposure_2->setChecked(true);
	}
	else
	{
		switch (setConfig.lastChooseLight2)
		{
		case 0:
			ui->rbtn_ruoguang_2->setChecked(true);
			rbtn_ruoguang_2_checked(true);
			break;
		case 1:
			ui->rbtn_zhongguang_2->setChecked(true);
			rbtn_zhongguang_2_checked(true);
			break;
		case 2:
			ui->rbtn_qiangguang_2->setChecked(true);
			rbtn_qiangguang_2_checked(true);
			break;
		default:
			ui->rbtn_ruoguang_2->setChecked(true);
			rbtn_ruoguang_2_checked(true);
			break;
		}
	}

	// 任一相机开启自动曝光即显示信息标签
	updateExposureInfoVisibility();
}

void EdgeWidthDetection::ini_clickableTitle()
{
	// 初始化标题label
	clickableTitle = new rw::rqw::ClickableLabel(this);
	auto layoutTitle = ui->groupBox_head->layout();
	layoutTitle->replaceWidget(ui->label_title, clickableTitle);
	delete ui->label_title;
	clickableTitle->setText("宽度检测");
	clickableTitle->setStyleSheet("QLabel {font-size: 30px;font-weight: bold;color: rgb(255, 255, 255);padding: 5px 5px;border-bottom: 2px solid #cccccc;}");

	// 版本号显示（读取 exe 同目录 build.version，缺失时回退到编译期嵌入的版本号）
	ui->label_VersionInfo->setText(QString("v%1").arg(ewd::getDisplayVersion()));
}

void EdgeWidthDetection::build_DlgCloseForm()
{
	_dlgCloseForm = new DlgCloseForm(this);
}

void EdgeWidthDetection::initializeComponents()
{
	build_ui();

	build_camera();

	build_plcController();

	build_connect();

#ifndef BUILD_WITHOUT_HARDWARE
#endif
}

void EdgeWidthDetection::build_camera()
{
	auto& cameraModules = Modules::getInstance().cameraModule;
	auto errors = cameraModules.getBuildResults();
	updateCameraLabelState(1, true);
	updateCameraLabelState(2, true);
	updateCameraLabelState(3, true);

	for (const auto& error : errors)
	{
		auto index = static_cast<int>(error);
		updateCameraLabelState(index, false);
	}
}

void EdgeWidthDetection::build_plcController()
{
	auto& plcController = Modules::getInstance().plcController;
	auto build_Result = plcController.getBuildResult();
	updateCameraLabelState(0, build_Result);
}

void EdgeWidthDetection::updateCameraLabelState(int cameraIndex, bool state)
{
	// 连接成功：绿色文字；连接失败：白色文字 + 橙色背景
	//（连接失败时标题栏背景会变红，红字会重叠看不清，故失败项改用橙色底白字）
	const QString successStyle = "QLabel{color:rgb(0, 230, 0);font-size: 18px;font-weight: bold;padding: 5px 5px;}";
	const QString failStyle = "QLabel{color:rgb(255, 255, 255);background-color:rgb(255, 165, 0);font-size: 18px;font-weight: bold;padding: 5px 5px;}";

	switch (cameraIndex)
	{
	case 0:
		_plcConnected = state;
		ui->label_plc1State->setText(state ? "连接成功" : "连接失败");
		ui->label_plc1State->setStyleSheet(state ? successStyle : failStyle);
		break;
	case 1:
		_camera1Connected = state;
		ui->label_camera1State->setText(state ? "连接成功" : "连接失败");
		ui->label_camera1State->setStyleSheet(state ? successStyle : failStyle);
		break;
	case 2:
		_camera2Connected = state;
		ui->label_camera2State->setText(state ? "连接成功" : "连接失败");
		ui->label_camera2State->setStyleSheet(state ? successStyle : failStyle);
		break;
	default:
		return;
	}

	updateHeadBackground();
}

void EdgeWidthDetection::updateHeadBackground()
{
	// 任一相机或PLC未连接成功时，标题栏背景置红作醒目提示；全部正常时恢复默认灰色
	const bool allConnected = _camera1Connected && _camera2Connected && _plcConnected;
	const QString backgroundColor = allConnected ? "rgb(81, 81, 81)" : "rgb(230, 0, 0)";
	ui->groupBox_head->setStyleSheet(QString(
		"QGroupBox {"
		"font-size: 20px;"
		"font-weight: bold;"
		"background-color: %1;"
		"border: 1px solid #e0e0e0;"
		"border-radius: 15px;"
		"}"
		"QGroupBox::title {"
		"subcontrol-origin: margin;"
		"left: 10px;"
		"padding: 0 5px;"
		"color: #2c3e50;"
		"}").arg(backgroundColor));
}

void EdgeWidthDetection::onUpdateStatisticalInfoUI()
{
	auto& statisticalInfo = Modules::getInstance().runtimeInfoModule.statisticalInfo;
	ui->label_camera1PhotoValue->setText(QString::number(statisticalInfo.camera1PhotoCount.load()));
	ui->label_camera1UnrecognizedValue->setText(QString::number(statisticalInfo.camera1UnrecognizedCount.load()));
	ui->label_camera2PhotoValue->setText(QString::number(statisticalInfo.camera2PhotoCount.load()));
	ui->label_camera2UnrecognizedValue->setText(QString::number(statisticalInfo.camera2UnrecognizedCount.load()));
}

void EdgeWidthDetection::onUpdatePLCWarnningInfoUI(uint16_t warnningInfo)
{
	if (0 == warnningInfo)
	{
		ui->label_warnningInfo->setText("无报警");
	}
	else if (1 == warnningInfo)
	{
		ui->label_warnningInfo->setText("到达限位报警");
	}
}

void EdgeWidthDetection::onCameraDisplay(size_t index, QPixmap image)
{
	if (1 == index)
	{
		ui->label_imgDisplay_1->setPixmap(image.scaled(ui->label_imgDisplay_1->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
	}
	else if (2 == index)
	{
		ui->label_imgDisplay_2->setPixmap(image.scaled(ui->label_imgDisplay_2->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
	}
}

void EdgeWidthDetection::lb_title_clicked()
{
	if (0 != minimizeCount)
	{
		minimizeCount--;
	}
	else if (0 >= minimizeCount)
	{
		// 最小化主窗体
		this->showMinimized();
		if (_picturesViewer && _picturesViewer->isVisible())
			_picturesViewer->showMinimized();
		minimizeCount = 3; // 重置最小化计数器
	}

}

void EdgeWidthDetection::pbtn_exit_clicked()
{
#ifdef NDEBUG
	if (_dlgCloseForm)
	{
		_dlgCloseForm->exec();
	}
#else
	this->close();
#endif
}

void EdgeWidthDetection::pbtn_set_clicked()
{
	auto& _dlgProductSet = Modules::getInstance().uiModule._dlgProductSet;
	_dlgProductSet->setFixedSize(this->width(), this->height());
	_dlgProductSet->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	_dlgProductSet->exec();
}

void EdgeWidthDetection::pbtn_Modbus_clicked()
{
	auto& _dlgModbus = Modules::getInstance().uiModule._dlgModbus;
	_dlgModbus->setFixedSize(this->width(), this->height());
	_dlgModbus->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	_dlgModbus->exec();
}

void EdgeWidthDetection::rbtn_debug_checked(bool checked)
{
	auto isRuning = ui->rbtn_removeFunc->isChecked();

	auto& runningState = Modules::getInstance().runtimeInfoModule.runningState;
	auto& camera1 = Modules::getInstance().cameraModule.camera1;
	auto& camera2 = Modules::getInstance().cameraModule.camera2;
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	if (!isRuning) {
		if (checked) {
			runningState = RunningState::Debug;
			if (camera1)
			{
				camera1->setTriggerState(false);
				camera1->setFrameRate(setConfig.debugFrame1);
			}
			if (camera2)
			{
				camera2->setTriggerState(false);
				camera2->setFrameRate(setConfig.debugFrame2);
			}
		}
		else {
			runningState = RunningState::Stop;
		}
	}
	else {
		ui->rbtn_debug->setChecked(false);
	}
}

void EdgeWidthDetection::rbtn_removeFunc_checked(bool checked)
{
	auto& runningState = Modules::getInstance().runtimeInfoModule.runningState;
	auto& camera1 = Modules::getInstance().cameraModule.camera1;
	auto& camera2 = Modules::getInstance().cameraModule.camera2;
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	if (checked)
	{
		// 先切相机到触发模式，再翻转运行状态：
		// 保证状态翻转前到达的调试连续帧仍被打 Debug 戳走调试分发，不计数、不写PLC
		if (camera1)
		{
			camera1->setTriggerState(true);
			camera1->setFrameRate(setConfig.openRemoveFrame1);
		}
		if (camera2)
		{
			camera2->setTriggerState(true);
			camera2->setFrameRate(setConfig.openRemoveFrame2);
		}
		runningState = RunningState::OpenRemoveFunc;
		ui->rbtn_debug->setChecked(false);
	}
	else
	{
		runningState = RunningState::Stop;
	}
}

void EdgeWidthDetection::pbtn_resetProduct_clicked()
{
	// 产量清零：清空两相机的拍照总量与未识别总量统计
	auto& statisticalInfo = Modules::getInstance().runtimeInfoModule.statisticalInfo;
	statisticalInfo.camera1PhotoCount = 0;
	statisticalInfo.camera1UnrecognizedCount = 0;
	statisticalInfo.camera2PhotoCount = 0;
	statisticalInfo.camera2UnrecognizedCount = 0;

	onUpdateStatisticalInfoUI();
}

void EdgeWidthDetection::pbtn_openSaveLocation_clicked()
{
	auto& imageSaveEngine = Modules::getInstance().imgSaveModule.imageSaveEngine;
	QString imageSavePath = imageSaveEngine->getRootPath();

	// 取上一级目录
	QDir dir(imageSavePath);
	dir.cdUp();
	QString parentPath = dir.absolutePath();

	_picturesViewer->setRootPath(parentPath);
	_picturesViewer->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	_picturesViewer->show();
}

void EdgeWidthDetection::rbtn_ruoguang_checked(bool checked)
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	auto& camera1 = Modules::getInstance().cameraModule.camera1;

	setConfig.autoExposureEnabled1 = false;
	_cam1ExposureInfo.clear();
	refreshExposureInfo();
	updateExposureInfoVisibility();
	emit autoExposureToggled(false);

	if (camera1)
	{
		camera1->setExposureTime(setConfig.ruoguang1);
	}
	setConfig.lastChooseLight1 = 0;
}

void EdgeWidthDetection::rbtn_zhongguang_checked(bool checked)
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	auto& camera1 = Modules::getInstance().cameraModule.camera1;

	setConfig.autoExposureEnabled1 = false;
	_cam1ExposureInfo.clear();
	refreshExposureInfo();
	updateExposureInfoVisibility();
	emit autoExposureToggled(false);

	if (camera1)
	{
		camera1->setExposureTime(setConfig.zhongguang1);
	}
	setConfig.lastChooseLight1 = 1;
}

void EdgeWidthDetection::rbtn_qiangguang_checked(bool checked)
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	auto& camera1 = Modules::getInstance().cameraModule.camera1;

	setConfig.autoExposureEnabled1 = false;
	_cam1ExposureInfo.clear();
	refreshExposureInfo();
	updateExposureInfoVisibility();
	emit autoExposureToggled(false);

	if (camera1)
	{
		camera1->setExposureTime(setConfig.qiangguang1);
	}
	setConfig.lastChooseLight1 = 2;
}

void EdgeWidthDetection::ckb_saveImg_checked(bool checked)
{
	auto& maiLiDingZiConfig = Modules::getInstance().configManagerModule.edgeWidthDetectionConfig;
	maiLiDingZiConfig.isSaveImg = checked;
}

void EdgeWidthDetection::ckb_autoExposure_checked(bool checked)
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	setConfig.autoExposureEnabled1 = checked;
	if (!checked)
	{
		_cam1ExposureInfo.clear();
		refreshExposureInfo();
	}
	updateExposureInfoVisibility();
	emit autoExposureToggled(checked);
}

void EdgeWidthDetection::onAutoExposureInfo(double targetExposure, double meanIntensity,
	double overRatio, double underRatio)
{
	_cam1ExposureInfo = QString("相机1 均值:%1 过曝:%2% 欠曝:%3% 目标曝光:%4")
		.arg(meanIntensity, 0, 'f', 1)
		.arg(overRatio * 100.0, 0, 'f', 1)
		.arg(underRatio * 100.0, 0, 'f', 1)
		.arg(targetExposure, 0, 'f', 0);
	refreshExposureInfo();
}

void EdgeWidthDetection::onAutoExposureInfo2(double targetExposure, double meanIntensity,
	double overRatio, double underRatio)
{
	_cam2ExposureInfo = QString("相机2 均值:%1 过曝:%2% 欠曝:%3% 目标曝光:%4")
		.arg(meanIntensity, 0, 'f', 1)
		.arg(overRatio * 100.0, 0, 'f', 1)
		.arg(underRatio * 100.0, 0, 'f', 1)
		.arg(targetExposure, 0, 'f', 0);
	refreshExposureInfo();
}

void EdgeWidthDetection::refreshExposureInfo()
{
	QString text;
	if (!_cam1ExposureInfo.isEmpty() && !_cam2ExposureInfo.isEmpty())
	{
		text = _cam1ExposureInfo + "\n" + _cam2ExposureInfo;
	}
	else if (!_cam1ExposureInfo.isEmpty())
	{
		text = _cam1ExposureInfo;
	}
	else if (!_cam2ExposureInfo.isEmpty())
	{
		text = _cam2ExposureInfo;
	}
	ui->label_warnningInfo->setText(text);
}

void EdgeWidthDetection::updateExposureInfoVisibility()
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	bool anyAuto = setConfig.autoExposureEnabled1 || setConfig.autoExposureEnabled2;
	ui->label_warnningInfo->setVisible(anyAuto);
}

void EdgeWidthDetection::rbtn_ruoguang_2_checked(bool checked)
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	auto& camera2 = Modules::getInstance().cameraModule.camera2;

	setConfig.autoExposureEnabled2 = false;
	_cam2ExposureInfo.clear();
	refreshExposureInfo();
	updateExposureInfoVisibility();
	emit autoExposureToggled2(false);

	if (camera2)
	{
		camera2->setExposureTime(setConfig.ruoguang2);
	}
	setConfig.lastChooseLight2 = 0;
}

void EdgeWidthDetection::rbtn_zhongguang_2_checked(bool checked)
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	auto& camera2 = Modules::getInstance().cameraModule.camera2;

	setConfig.autoExposureEnabled2 = false;
	_cam2ExposureInfo.clear();
	refreshExposureInfo();
	updateExposureInfoVisibility();
	emit autoExposureToggled2(false);

	if (camera2)
	{
		camera2->setExposureTime(setConfig.zhongguang2);
	}
	setConfig.lastChooseLight2 = 1;
}

void EdgeWidthDetection::rbtn_qiangguang_2_checked(bool checked)
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	auto& camera2 = Modules::getInstance().cameraModule.camera2;

	setConfig.autoExposureEnabled2 = false;
	_cam2ExposureInfo.clear();
	refreshExposureInfo();
	updateExposureInfoVisibility();
	emit autoExposureToggled2(false);

	if (camera2)
	{
		camera2->setExposureTime(setConfig.qiangguang2);
	}
	setConfig.lastChooseLight2 = 2;
}

void EdgeWidthDetection::ckb_autoExposure_2_checked(bool checked)
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	setConfig.autoExposureEnabled2 = checked;
	if (!checked)
	{
		_cam2ExposureInfo.clear();
		refreshExposureInfo();
	}
	updateExposureInfoVisibility();
	emit autoExposureToggled2(checked);
}