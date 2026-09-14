#include "EdgeWidthDetection.h"

#include <QDir>
#include <QFile>
#include <QFutureWatcher>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QProcess>
#include <QStringConverter>
#include <QTextStream>
#include <QTimer>
#include <QtConcurrent/qtconcurrentrun.h>
#include <cstdlib>
#include <functional>

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

namespace
{
	// ================= 主界面拍照/切刀面板点位（modbus_main.csv）================

	// CSV 表头（首行与此一致时跳过）
	const char* kMainUiCsvHeader = "名称,类型,协议地址,读写";

	// 约定的 17 个面板点位名，CSV 名称列必须在此列表中且各出现一次（严格校验）
	const QStringList kMainUiPointNames = {
		QStringLiteral("设定拍照长度"),
		QStringLiteral("实际拍照值"),
		QStringLiteral("白料长"),
		QStringLiteral("袋长"),
		QStringLiteral("编码器当前位置"),
		QStringLiteral("启动"),
		QStringLiteral("停止"),
		QStringLiteral("系统标志"),
		QStringLiteral("切刀一直点动速度"),
		QStringLiteral("切刀单次点动速度"),
		QStringLiteral("自动速度"),
		QStringLiteral("切刀计算移动量"),
		QStringLiteral("切刀实际移动量"),
		QStringLiteral("切刀当前位置"),
		QStringLiteral("切刀补偿"),
		QStringLiteral("切刀正限位"),
		QStringLiteral("切刀负限位"),
	};

	// 可点击（可写数值）标签样式：蓝色加粗下划线 + 手型光标，提示可点击
	const char* kMainClickableValueStyle =
		"QLabel {"
		"    font-size: 16px;"
		"    font-weight: bold;"
		"    color: #1565C0;"
		"    border: none;"
		"    text-decoration: underline;"
		"}";

	// 启动/停止按钮「已写入 1」状态的样式（绿底）；关状态直接用 .ui 中的原始样式表
	const char* kToggleBtnOnStyle =
		"QPushButton {"
		"    padding: 4px 8px;"
		"    border: 1px solid #00a040;"
		"    border-radius: 3px;"
		"    background-color: #00e060;"
		"    color: #003300;"
		"    font-size: 16px;"
		"    font-weight: bold;"
		"}"
		"QPushButton:pressed { background-color: #00b050; }";

	// 切刀补偿按钮两种状态的样式（开=绿底，关=红底）
	const char* kCutCompensateOnStyle =
		"QPushButton {"
		"    padding: 6px 10px;"
		"    border: 1px solid #00a040;"
		"    border-radius: 3px;"
		"    background-color: #00e060;"
		"    color: #003300;"
		"    font-size: 16px;"
		"    font-weight: bold;"
		"}"
		"QPushButton:pressed { background-color: #00b050; }";
	const char* kCutCompensateOffStyle =
		"QPushButton {"
		"    padding: 6px 10px;"
		"    border: 1px solid #a03030;"
		"    border-radius: 3px;"
		"    background-color: #e05050;"
		"    color: #ffffff;"
		"    font-size: 16px;"
		"    font-weight: bold;"
		"}"
		"QPushButton:pressed { background-color: #b04040; }";

	// 解析类型列，宽容大小写；返回 false 表示无法识别（严格校验，非法值会导致启动失败）
	bool parseMainPointType(const QString& text, EdgeWidthDetection::MainPointType& out)
	{
		const QString s = text.trimmed().toLower();
		if (s == "float" || s == "real")
		{
			out = EdgeWidthDetection::MainPointType::Float;
			return true;
		}
		if (s == "dint" || s == "int32" || s == "dword")
		{
			out = EdgeWidthDetection::MainPointType::Dint;
			return true;
		}
		if (s == "bool" || s == "bit" || s == "coil")
		{
			out = EdgeWidthDetection::MainPointType::Bool;
			return true;
		}
		return false;
	}

	QString mainPointTypeToString(EdgeWidthDetection::MainPointType type)
	{
		switch (type)
		{
		case EdgeWidthDetection::MainPointType::Dint: return QStringLiteral("DINT");
		case EdgeWidthDetection::MainPointType::Bool: return QStringLiteral("BOOL");
		default: return QStringLiteral("float");
		}
	}

	// 解析读写列：返回 false 表示无法识别（严格校验）
	bool parseMainWritable(const QString& text, bool& out)
	{
		const QString s = text.trimmed().toLower();
		if (s == QStringLiteral("读写") || s == "rw" || s == "write" || s == "w")
		{
			out = true;
			return true;
		}
		if (s == QStringLiteral("只读") || s == "ro" || s == "read" || s == "r")
		{
			out = false;
			return true;
		}
		return false;
	}

	// 解析协议地址列（实际读写 Modbus 的地址），必须是非负整数
	int parseMainProtocolAddress(const QString& text)
	{
		bool ok = false;
		const int addr = text.trimmed().toInt(&ok);
		return (ok && addr >= 0) ? addr : -1;
	}

	struct MainPointDefault
	{
		const char* name;
		EdgeWidthDetection::MainPointType type;
		int protocolAddress;
		bool writable;
	};

	// 内置默认表，协议地址与《通讯地址》约定一致；仅在 modbus_main.csv 缺失时使用
	const MainPointDefault kMainUiDefaults[] = {
		{ "设定拍照长度",		EdgeWidthDetection::MainPointType::Float,	1002,	true  },
		{ "实际拍照值",			EdgeWidthDetection::MainPointType::Float,	3000,	false },
		{ "白料长",				EdgeWidthDetection::MainPointType::Float,	3040,	false },
		{ "袋长",				EdgeWidthDetection::MainPointType::Float,	3030,	false },
		{ "编码器当前位置",		EdgeWidthDetection::MainPointType::Float,	2000,	false },
		{ "启动",				EdgeWidthDetection::MainPointType::Bool,	12954,	true  },
		{ "停止",				EdgeWidthDetection::MainPointType::Bool,	12955,	true  },
		{ "系统标志",			EdgeWidthDetection::MainPointType::Bool,	12956,	false },
		{ "切刀一直点动速度",	EdgeWidthDetection::MainPointType::Float,	1000,	true  },
		{ "切刀单次点动速度",	EdgeWidthDetection::MainPointType::Float,	1038,	true  },
		{ "自动速度",			EdgeWidthDetection::MainPointType::Float,	1026,	true  },
		{ "切刀计算移动量",		EdgeWidthDetection::MainPointType::Float,	3042,	false },
		{ "切刀实际移动量",		EdgeWidthDetection::MainPointType::Float,	3060,	false },
		{ "切刀当前位置",		EdgeWidthDetection::MainPointType::Float,	2004,	false },
		{ "切刀补偿",			EdgeWidthDetection::MainPointType::Bool,	3000,	true  },
		{ "切刀正限位",			EdgeWidthDetection::MainPointType::Bool,	1207,	false },
		{ "切刀负限位",			EdgeWidthDetection::MainPointType::Bool,	1204,	false },
	};

	// 设置圆形指示灯颜色：onColor/offColor 为状态色，读取失败时置灰
	void setLampColor(QLabel* lamp, bool readOk, bool on, const QString& onColor, const QString& offColor)
	{
		const QString color = !readOk ? QStringLiteral("#9e9e9e") : (on ? onColor : offColor);
		lamp->setStyleSheet(QStringLiteral(
			"QLabel { background-color: %1; border: 1px solid #7a7a7a; border-radius: 15px; }").arg(color));
	}
}


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
	build_mainUiModbus();

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

void EdgeWidthDetection::onDiskSpaceStateChanged(bool enough, double freeGB)
{
	if (enough)
	{
		QMessageBox::information(this, QStringLiteral("提示"),
			QStringLiteral("存图磁盘剩余空间已恢复至 %1 GB，存图功能已自动恢复。")
			.arg(QString::number(freeGB, 'f', 2)));
	}
	else
	{
		QMessageBox::warning(this, QStringLiteral("磁盘空间不足"),
			QStringLiteral("存图磁盘剩余空间不足（仅剩 %1 GB，低于 10GB），已自动暂停存图！\n请及时清理磁盘或转移历史图像，空间恢复后将自动恢复存图。")
			.arg(QString::number(freeGB, 'f', 2)));
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

void EdgeWidthDetection::build_mainUiModbus()
{
	loadMainUiPoints();

	// 四个可写数值行替换为可点击标签（点击弹出数字键盘写入）
	clk_setPhotoLength = replaceWithClickableValue(ui->label_setPhotoLengthValue);
	clk_cutJogContSpeed = replaceWithClickableValue(ui->label_cutJogContSpeedValue);
	clk_cutJogOnceSpeed = replaceWithClickableValue(ui->label_cutJogOnceSpeedValue);
	clk_autoSpeed = replaceWithClickableValue(ui->label_autoSpeedValue);

	// 数值行：点位名 -> 当前值标签（轮询刷新时按名更新）
	_mainUiValueLabels.insert(QStringLiteral("设定拍照长度"), clk_setPhotoLength);
	_mainUiValueLabels.insert(QStringLiteral("实际拍照值"), ui->label_actualPhotoValue);
	_mainUiValueLabels.insert(QStringLiteral("白料长"), ui->label_whiteLengthValue);
	_mainUiValueLabels.insert(QStringLiteral("袋长"), ui->label_bagLengthValue);
	_mainUiValueLabels.insert(QStringLiteral("编码器当前位置"), ui->label_encoderPosValue);
	_mainUiValueLabels.insert(QStringLiteral("切刀一直点动速度"), clk_cutJogContSpeed);
	_mainUiValueLabels.insert(QStringLiteral("切刀单次点动速度"), clk_cutJogOnceSpeed);
	_mainUiValueLabels.insert(QStringLiteral("自动速度"), clk_autoSpeed);
	_mainUiValueLabels.insert(QStringLiteral("切刀计算移动量"), ui->label_cutCalcMoveValue);
	_mainUiValueLabels.insert(QStringLiteral("切刀实际移动量"), ui->label_cutActualMoveValue);
	_mainUiValueLabels.insert(QStringLiteral("切刀当前位置"), ui->label_cutPosValue);

	QObject::connect(ui->pbtn_start, &QPushButton::clicked,
		this, &EdgeWidthDetection::pbtn_start_clicked);
	QObject::connect(ui->pbtn_stop, &QPushButton::clicked,
		this, &EdgeWidthDetection::pbtn_stop_clicked);
	QObject::connect(ui->pbtn_cutCompensate, &QPushButton::clicked,
		this, &EdgeWidthDetection::pbtn_cutCompensate_clicked);
	QObject::connect(clk_setPhotoLength, &rw::rqw::ClickableLabel::clicked, this, [this]()
		{
			writeMainValue(QStringLiteral("设定拍照长度"));
		});
	QObject::connect(clk_cutJogContSpeed, &rw::rqw::ClickableLabel::clicked, this, [this]()
		{
			writeMainValue(QStringLiteral("切刀一直点动速度"));
		});
	QObject::connect(clk_cutJogOnceSpeed, &rw::rqw::ClickableLabel::clicked, this, [this]()
		{
			writeMainValue(QStringLiteral("切刀单次点动速度"));
		});
	QObject::connect(clk_autoSpeed, &rw::rqw::ClickableLabel::clicked, this, [this]()
		{
			writeMainValue(QStringLiteral("自动速度"));
		});

	QObject::connect(&_mainUiRefreshTimer, &QTimer::timeout,
		this, &EdgeWidthDetection::onMainUiRefreshTimeout);
	_mainUiRefreshTimer.setInterval(500);
	_mainUiRefreshTimer.start();

	// 初始按补偿关闭显示，首次轮询后校正为真实状态
	updateCutCompensateButton();
}

void EdgeWidthDetection::loadMainUiPoints()
{
	// modbus_main.csv 每行：名称,类型,协议地址,读写
	// 与 DlgModbus 的 modbus.csv 相互独立；名称必须是约定的 17 个点位名且各出现一次
	// 严格校验：任何一行格式非法都会弹窗提示并退出程序，防止错误配置静默生效
	QFile file(globalPath.modbusMainCsvPath);
	if (!file.exists())
	{
		qDebug() << "modbus_main.csv 不存在，按内置默认表生成:" << globalPath.modbusMainCsvPath;
		_mainUiPoints.clear();
		for (const auto& d : kMainUiDefaults)
		{
			MainUiPoint info;
			info.name = QString::fromUtf8(d.name);
			info.type = d.type;
			info.protocolAddress = d.protocolAddress;
			info.writable = d.writable;
			_mainUiPoints.push_back(info);
		}
		saveMainUiPoints();
		return;
	}

	if (!file.open(QIODevice::ReadOnly))
	{
		abortOnMainUiLoadErrors({ QStringLiteral("modbus_main.csv 无法打开（可能被占用或无权限）") });
		return;
	}

	const QByteArray raw = file.readAll();
	file.close();

	// 编码自动识别：UTF-8（带/不带 BOM），含非法 UTF-8 序列时按系统编码（GBK，兼容 Excel 另存的 CSV）
	QString text = QString::fromUtf8(raw);
	if (text.contains(QChar::ReplacementCharacter))
	{
		QStringDecoder decoder(QStringConverter::System);
		text = decoder.decode(raw);
	}
	// 去掉 BOM（trimmed() 不会去掉 U+FEFF，否则会污染首行首列）
	if (text.startsWith(QChar(0xFEFF)))
	{
		text.remove(0, 1);
	}

	_mainUiPoints.clear();
	QStringList errors;
	QStringList seenNames;
	const QStringList lines = text.split('\n');
	for (int lineNo = 0; lineNo < lines.size(); ++lineNo)
	{
		const QString line = lines[lineNo].trimmed();
		if (line.isEmpty())
		{
			continue;
		}

		const QStringList fields = line.split(',');
		if (fields.value(0).trimmed() == QStringLiteral("名称"))	// 跳过表头
		{
			continue;
		}

		auto addError = [&](const QString& reason)
		{
			errors << QStringLiteral("第 %1 行「%2」：%3").arg(lineNo + 1).arg(line, reason);
		};

		if (fields.size() != 4)
		{
			addError(QStringLiteral("应为 4 列（名称,类型,协议地址,读写），实际为 %1 列").arg(fields.size()));
			continue;
		}

		MainUiPoint info;
		info.name = fields[0].trimmed();
		if (!kMainUiPointNames.contains(info.name))
		{
			addError(QStringLiteral("名称「%1」无法识别，应为约定的面板点位名之一：%2")
				.arg(info.name, kMainUiPointNames.join(QStringLiteral("、"))));
			continue;
		}
		if (seenNames.contains(info.name))
		{
			addError(QStringLiteral("名称「%1」重复出现").arg(info.name));
			continue;
		}
		seenNames << info.name;

		if (!parseMainPointType(fields[1], info.type))
		{
			addError(QStringLiteral("类型「%1」无法识别，应为 float / DINT / BOOL").arg(fields[1].trimmed()));
			continue;
		}

		info.protocolAddress = parseMainProtocolAddress(fields[2]);
		if (info.protocolAddress < 0)
		{
			addError(QStringLiteral("协议地址「%1」无效，应为非负整数").arg(fields[2].trimmed()));
			continue;
		}

		if (!parseMainWritable(fields[3], info.writable))
		{
			addError(QStringLiteral("读写列「%1」无法识别，应为「读写」或「只读」").arg(fields[3].trimmed()));
			continue;
		}

		_mainUiPoints.push_back(info);
	}

	// 完整性校验：约定的点位必须全部出现（防止漏行导致面板某行永远不刷新）
	for (const auto& name : kMainUiPointNames)
	{
		if (!seenNames.contains(name))
		{
			errors << QStringLiteral("缺少点位「%1」").arg(name);
		}
	}

	if (!errors.isEmpty())
	{
		_mainUiPoints.clear();
		abortOnMainUiLoadErrors(errors);
	}
}

void EdgeWidthDetection::abortOnMainUiLoadErrors(const QStringList& errors)
{
	qWarning() << "modbus_main.csv 配置错误，程序退出:\n" << errors.join('\n');

	// 最多展示前 10 条，避免弹窗过长
	QStringList shown = errors.mid(0, 10);
	QString detail = shown.join('\n');
	if (errors.size() > shown.size())
	{
		detail += QStringLiteral("\n……共 %1 处错误").arg(errors.size());
	}

	QMessageBox::critical(nullptr, QStringLiteral("Modbus 配置错误"),
		QStringLiteral("%1 配置存在以下错误：\n\n%2\n\n请修正该文件后重新启动程序，程序即将退出。")
		.arg(globalPath.modbusMainCsvPath, detail));
	std::exit(EXIT_FAILURE);
}

void EdgeWidthDetection::saveMainUiPoints()
{
	QFile file(globalPath.modbusMainCsvPath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
	{
		qDebug() << "modbus_main.csv 写入失败:" << globalPath.modbusMainCsvPath;
		return;
	}

	// UTF-8 带 BOM，保证 Excel 直接打开不乱码
	QTextStream out(&file);
	out.setEncoding(QStringConverter::Utf8);
	out.setGenerateByteOrderMark(true);

	out << QString::fromUtf8(kMainUiCsvHeader) << '\n';
	for (const auto& point : _mainUiPoints)
	{
		out << point.name << ','
			<< mainPointTypeToString(point.type) << ','
			<< point.protocolAddress << ','
			<< (point.writable ? QStringLiteral("读写") : QStringLiteral("只读")) << '\n';
	}
}

const EdgeWidthDetection::MainUiPoint* EdgeWidthDetection::findMainUiPoint(const QString& name) const
{
	for (const auto& point : _mainUiPoints)
	{
		if (point.name == name)
		{
			return &point;
		}
	}
	return nullptr;
}

bool EdgeWidthDetection::checkManualWriteReady()
{
	if (!Modules::getInstance().plcController.manualWriteEnabled.load())
	{
		QMessageBox::information(this, QStringLiteral("提示"),
			QStringLiteral("Modbus 手动写入已被禁用（可在「通讯」界面右上角勾选「允许写入」开启）"));
		return false;
	}
	if (!Modules::getInstance().plcController.plcControllerScheduler)
	{
		QMessageBox::information(this, QStringLiteral("警告"), QStringLiteral("PLC未连接"));
		return false;
	}
	return true;
}

bool EdgeWidthDetection::writeMainCoil(const QString& pointName, bool value)
{
	if (!checkManualWriteReady())
	{
		return false;
	}

	const auto* point = findMainUiPoint(pointName);
	if (!point || point->type != MainPointType::Bool || !point->writable)
	{
		QMessageBox::warning(this, QStringLiteral("警告"),
			QStringLiteral("modbus_main.csv 中未找到可写 BOOL 点位「%1」，请检查点位表").arg(pointName));
		return false;
	}

	auto& scheduler = Modules::getInstance().plcController.plcControllerScheduler;
	const auto addr = static_cast<rw::hoem::Address16>(point->protocolAddress);
	if (!scheduler->writeCoilAsync(addr, value).get())
	{
		QMessageBox::warning(this, QStringLiteral("警告"),
			pointName + (value ? QStringLiteral(" 置1失败") : QStringLiteral(" 置0失败")));
		return false;
	}
	return true;
}

void EdgeWidthDetection::writeMainValue(const QString& pointName)
{
	if (!checkManualWriteReady())
	{
		return;
	}

	const auto* point = findMainUiPoint(pointName);
	if (!point || point->type == MainPointType::Bool || !point->writable)
	{
		QMessageBox::warning(this, QStringLiteral("警告"),
			QStringLiteral("modbus_main.csv 中未找到可写数值点位「%1」，请检查点位表").arg(pointName));
		return;
	}

	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept != QDialog::Accepted)
	{
		return;
	}

	const auto value = numKeyBord.getValue();
	const auto addr = static_cast<rw::hoem::Address16>(point->protocolAddress);
	auto& scheduler = Modules::getInstance().plcController.plcControllerScheduler;

	bool success = false;
	if (point->type == MainPointType::Float)
	{
		success = scheduler->writeFloatRegisterAsync(
			addr, value.toFloat(), rw::hoem::Endianness::LittleEndian).get();
	}
	else if (point->type == MainPointType::Dint)
	{
		success = scheduler->writeUInt32RegisterAsync(
			addr, static_cast<rw::hoem::UInt32>(static_cast<int32_t>(value.toInt())),
			rw::hoem::Endianness::LittleEndian).get();
	}

	if (success)
	{
		// 写入成功后立即刷新一次显示
		onMainUiRefreshTimeout();
	}
	else
	{
		QMessageBox::warning(this, QStringLiteral("警告"), pointName + QStringLiteral(" 写入失败"));
	}
}

rw::rqw::ClickableLabel* EdgeWidthDetection::replaceWithClickableValue(QLabel* oldLabel)
{
	auto* clickable = new rw::rqw::ClickableLabel(this);
	clickable->setText(oldLabel->text());
	clickable->setAlignment(oldLabel->alignment());
	clickable->setCursor(Qt::PointingHandCursor);
	clickable->setStyleSheet(QString::fromUtf8(kMainClickableValueStyle));
	// replaceWidget 会递归查找子布局（与 ini_clickableTitle 中标题标签的替换方式相同）
	oldLabel->parentWidget()->layout()->replaceWidget(oldLabel, clickable);
	delete oldLabel;
	return clickable;
}

void EdgeWidthDetection::updateCutCompensateButton()
{
	if (_cutCompensateOn)
	{
		ui->pbtn_cutCompensate->setText(QStringLiteral("切刀补偿开启"));
		ui->pbtn_cutCompensate->setStyleSheet(QString::fromUtf8(kCutCompensateOnStyle));
	}
	else
	{
		ui->pbtn_cutCompensate->setText(QStringLiteral("切刀补偿关闭"));
		ui->pbtn_cutCompensate->setStyleSheet(QString::fromUtf8(kCutCompensateOffStyle));
	}
}

void EdgeWidthDetection::pbtn_start_clicked()
{
	// 点动：点击向「启动」线圈写 1，50ms 后自动复位为 0；
	// 按钮颜色固定（.ui 中启动绿/停止红），运行状态以旁边的系统标志位灯为准
	if (writeMainCoil(QStringLiteral("启动"), true))
	{
		QTimer::singleShot(50, this, [this]()
			{
				writeMainCoil(QStringLiteral("启动"), false);
			});
	}
}

void EdgeWidthDetection::pbtn_stop_clicked()
{
	if (writeMainCoil(QStringLiteral("停止"), true))
	{
		QTimer::singleShot(50, this, [this]()
			{
				writeMainCoil(QStringLiteral("停止"), false);
			});
	}
}

void EdgeWidthDetection::pbtn_cutCompensate_clicked()
{
	// 切换开关：按当前状态取反写入；按钮文字/颜色由轮询读到的新状态刷新
	writeMainCoil(QStringLiteral("切刀补偿"), !_cutCompensateOn);
	onMainUiRefreshTimeout();
}

void EdgeWidthDetection::onMainUiRefreshTimeout()
{
	auto& scheduler = Modules::getInstance().plcController.plcControllerScheduler;
	if (!scheduler)
	{
		// PLC 未连接时数值行显示 --，指示灯置灰
		for (auto* lb : _mainUiValueLabels)
		{
			lb->setText(QStringLiteral("--"));
		}
		setLampColor(ui->label_systemFlag, false, false, QString(), QString());
		setLampColor(ui->label_cutPosLimit, false, false, QString(), QString());
		setLampColor(ui->label_cutNegLimit, false, false, QString(), QString());
		return;
	}

	if (_mainUiRefreshInFlight)
	{
		return;
	}
	_mainUiRefreshInFlight = true;

	struct PointResult
	{
		QString text;				// 数值行显示文本（读取失败时为空）
		bool ok{ false };			// 读取是否成功
		bool boolValue{ false };	// BOOL 点位当前值
	};

	// 发起全部点位的异步读取（float/DINT 小端，BOOL 读线圈），future 移到后台线程统一等待
	// std::function 要求可拷贝，future 为移动语义，故用 shared_ptr 包一层
	std::vector<std::function<PointResult()>> tasks;
	tasks.reserve(_mainUiPoints.size());
	for (const auto& point : _mainUiPoints)
	{
		const auto addr = static_cast<rw::hoem::Address16>(point.protocolAddress);
		switch (point.type)
		{
		case MainPointType::Float:
		{
			auto fut = std::make_shared<std::future<std::pair<float, bool>>>(
				scheduler->readFloatRegisterAsync(addr, rw::hoem::Endianness::LittleEndian));
			tasks.push_back([fut]() mutable
				{
					auto result = fut->get();
					return PointResult{
						result.second ? QString::number(result.first, 'f', 2) : QString(),
						result.second, false };
				});
			break;
		}
		case MainPointType::Dint:
		{
			auto fut = std::make_shared<std::future<std::pair<rw::hoem::UInt32, bool>>>(
				scheduler->readUInt32RegisterAsync(addr, rw::hoem::Endianness::LittleEndian));
			tasks.push_back([fut]() mutable
				{
					auto result = fut->get();
					return PointResult{
						result.second ? QString::number(static_cast<int32_t>(result.first)) : QString(),
						result.second, false };
				});
			break;
		}
		case MainPointType::Bool:
		{
			auto fut = std::make_shared<std::future<std::pair<bool, bool>>>(
				scheduler->readCoilAsync(addr));
			tasks.push_back([fut]() mutable
				{
					auto result = fut->get();
					return PointResult{ QString(), result.second, result.second && result.first };
				});
			break;
		}
		}
	}

	auto* watcher = new QFutureWatcher<QVector<PointResult>>(this);
	connect(watcher, &QFutureWatcher<QVector<PointResult>>::finished, this, [this, watcher]()
		{
			const auto results = watcher->result();
			watcher->deleteLater();
			_mainUiRefreshInFlight = false;

			for (int i = 0; i < results.size() && i < _mainUiPoints.size(); ++i)
			{
				const auto& point = _mainUiPoints[i];
				const auto& res = results[i];

				if (auto* lb = _mainUiValueLabels.value(point.name, nullptr))
				{
					lb->setText(res.ok ? res.text : QStringLiteral("--"));
				}
				else if (point.name == QStringLiteral("系统标志"))
				{
					// 系统标志：1 绿、0 红，读取失败置灰
					setLampColor(ui->label_systemFlag, res.ok, res.boolValue,
						QStringLiteral("#00c853"), QStringLiteral("red"));
				}
				else if (point.name == QStringLiteral("切刀正限位"))
				{
					// 限位灯：触发(1)亮绿，未触发(0)灰
					setLampColor(ui->label_cutPosLimit, res.ok, res.boolValue,
						QStringLiteral("#00e000"), QStringLiteral("#9e9e9e"));
				}
				else if (point.name == QStringLiteral("切刀负限位"))
				{
					setLampColor(ui->label_cutNegLimit, res.ok, res.boolValue,
						QStringLiteral("#00e000"), QStringLiteral("#9e9e9e"));
				}
				else if (point.name == QStringLiteral("切刀补偿"))
				{
					if (res.ok)
					{
						_cutCompensateOn = res.boolValue;
						updateCutCompensateButton();
					}
				}
				// 启动/停止点位无需显示，读取结果忽略
			}
		});
	watcher->setFuture(QtConcurrent::run([tasks = std::move(tasks)]() mutable
		{
			QVector<PointResult> results;
			results.reserve(static_cast<int>(tasks.size()));
			for (auto& task : tasks)
			{
				results.push_back(task());
			}
			return results;
		}));
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