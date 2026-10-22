#include "EdgeWidthDetection.h"

#include <QDate>
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

	// 约定的面板点位名，CSV 名称列必须在此列表中且各出现一次（严格校验）
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
		// 新增 PLC 操作列点位（拍照/切刀面板左侧）
		QStringLiteral("间隔袋数"),
		QStringLiteral("切刀移动量"),
		QStringLiteral("配方号"),
		QStringLiteral("配方自动选择"),
		QStringLiteral("配方170"),
		QStringLiteral("配方180"),
		QStringLiteral("配方185"),
		QStringLiteral("配方190"),
		QStringLiteral("配方195"),
		QStringLiteral("配方200"),
		QStringLiteral("配方205"),
		QStringLiteral("配方210"),
		QStringLiteral("配方220"),
		QStringLiteral("配方230"),
	};

	// 12 个配方按钮对应的配方值（与界面按钮顺序一致；150/160 仅占位、无业务逻辑，不连点击槽）
	const int kRecipeBtnValues[] = { 150, 160, 170, 180, 185, 190, 195, 200, 205, 210, 220, 230 };
	const int kRecipeBtnCount = static_cast<int>(sizeof(kRecipeBtnValues) / sizeof(kRecipeBtnValues[0]));

	// 可点击（可写数值）标签样式：蓝色加粗下划线 + 手型光标，提示可点击
	const char* kMainClickableValueStyle =
		"QLabel {"
		"    font-size: 24px;"
		"    font-weight: bold;"
		"    color: #1565C0;"
		"    border: none;"
		"    text-decoration: underline;"
		"}";

	// 切刀补偿按钮两种状态的样式（开=绿底，关=红底）
	const char* kCutCompensateOnStyle =
		"QPushButton {"
		"    padding: 6px 10px;"
		"    border: 1px solid #00a040;"
		"    border-radius: 3px;"
		"    background-color: #00e060;"
		"    color: #003300;"
		"    font-size: 24px;"
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
		"    font-size: 24px;"
		"    font-weight: bold;"
		"}"
		"QPushButton:pressed { background-color: #b04040; }";

	// 12 个配方按钮专用样式：按下反馈做得更明显（深色底 + 加粗描边 + 文字变色），方便观察是否按下
	const char* kRecipeBtnOnStyle =
		"QPushButton {"
		"    border: 1px solid #00a040;"
		"    border-radius: 3px;"
		"    background-color: #00e060;"
		"    color: #003300;"
		"    font-size: 24px;"
		"    font-weight: bold;"
		"}"
		"QPushButton:pressed {"
		"    background-color: #007a2e;"
		"    border: 2px solid #004d1d;"
		"    color: #ffffff;"
		"}";
	const char* kRecipeBtnOffStyle =
		"QPushButton {"
		"    border: 1px solid #7a7a7a;"
		"    border-radius: 3px;"
		"    background-color: #e05050;"
		"    color: #ffffff;"
		"    font-size: 24px;"
		"    font-weight: bold;"
		"}"
		"QPushButton:pressed {"
		"    background-color: #8f2020;"
		"    border: 2px solid #5a1010;"
		"    color: #ffe08a;"
		"}";

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
		// 新增 PLC 操作列点位（拍照/切刀面板左侧）
		{ "间隔袋数",			EdgeWidthDetection::MainPointType::Dint,	1028,	true  },
		{ "切刀移动量",			EdgeWidthDetection::MainPointType::Float,	3066,	false },
		{ "配方号",				EdgeWidthDetection::MainPointType::Dint,	1642,	false },
		{ "配方自动选择",		EdgeWidthDetection::MainPointType::Bool,	3001,	true  },
		{ "配方170",			EdgeWidthDetection::MainPointType::Bool,	13053,	true  },
		{ "配方180",			EdgeWidthDetection::MainPointType::Bool,	13054,	true  },
		{ "配方185",			EdgeWidthDetection::MainPointType::Bool,	13055,	true  },
		{ "配方190",			EdgeWidthDetection::MainPointType::Bool,	13056,	true  },
		{ "配方195",			EdgeWidthDetection::MainPointType::Bool,	13057,	true  },
		{ "配方200",			EdgeWidthDetection::MainPointType::Bool,	13058,	true  },
		{ "配方205",			EdgeWidthDetection::MainPointType::Bool,	13059,	true  },
		{ "配方210",			EdgeWidthDetection::MainPointType::Bool,	13060,	true  },
		{ "配方220",			EdgeWidthDetection::MainPointType::Bool,	13061,	true  },
		{ "配方230",			EdgeWidthDetection::MainPointType::Bool,	13062,	true  },
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

	// 初始化图像查看器（只显示每个目录的前 200 张图片，避免图片过多时加载过慢、占用内存过大）
	_picturesViewer = new PictureViewerThumbnails(this);
	_picturesViewer->setViewerNum(200);

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
	// 只查看当天的图片文件夹（与 ImgSaveModule 的日期目录拼接方式保持一致）
	QDir dir;
	QString todayPath = dir.absoluteFilePath(
		globalPath.imageSaveRootPath + QDate::currentDate().toString("yyyy_MM_dd"));

	_picturesViewer->setRootPath(todayPath);
	_picturesViewer->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	_picturesViewer->show();
}

void EdgeWidthDetection::build_mainUiModbus()
{
	loadMainUiPoints();

	// PLC 操作列的可写数值行替换为可点击标签（点击弹出数字键盘写入）
	clk_plcSetPhotoLength = replaceWithClickableValue(ui->label_plcSetPhotoLengthValue);
	clk_plcBagCount = replaceWithClickableValue(ui->label_plcBagCountValue);

	// 数值行：点位名 -> 当前值标签（轮询刷新时按名更新）
	auto addValueLabel = [this](const QString& name, QLabel* label)
	{
		_mainUiValueLabels[name].append(label);
	};
	addValueLabel(QStringLiteral("设定拍照长度"), clk_plcSetPhotoLength);
	addValueLabel(QStringLiteral("实际拍照值"), ui->label_plcActualPhotoValue);
	addValueLabel(QStringLiteral("白料长"), ui->label_plcWhiteLengthValue);
	addValueLabel(QStringLiteral("切刀当前位置"), ui->label_plcCutPosValue);
	addValueLabel(QStringLiteral("间隔袋数"), clk_plcBagCount);
	addValueLabel(QStringLiteral("切刀移动量"), ui->label_plcCutMoveValue);

	QObject::connect(clk_plcSetPhotoLength, &rw::rqw::ClickableLabel::clicked, this, [this]()
		{
			writeMainValue(QStringLiteral("设定拍照长度"));
		});
	QObject::connect(clk_plcBagCount, &rw::rqw::ClickableLabel::clicked, this, [this]()
		{
			writeMainValue(QStringLiteral("间隔袋数"));
		});

	// 12 个配方按钮（150~230）：颜色仅由「配方号」点位驱动；150/160 仅 UI 占位、不连点击槽，
	// 其余 10 个按钮点击按各自线圈当前值取反写入
	QPushButton* const kRecipeBtns[] = {
		ui->pbtn_plcRecipe150, ui->pbtn_plcRecipe160, ui->pbtn_plcRecipe170,
		ui->pbtn_plcRecipe180, ui->pbtn_plcRecipe185, ui->pbtn_plcRecipe190,
		ui->pbtn_plcRecipe195, ui->pbtn_plcRecipe200, ui->pbtn_plcRecipe205,
		ui->pbtn_plcRecipe210, ui->pbtn_plcRecipe220, ui->pbtn_plcRecipe230 };
	for (auto* btn : kRecipeBtns)
	{
		_recipeButtons.append(btn);
		// 前两个为 150/160 占位按钮：无任何业务逻辑，跳过槽连接
		if (_recipeButtons.size() > 2)
		{
			QObject::connect(btn, &QPushButton::clicked,
				this, &EdgeWidthDetection::onPlcRecipeBtn_clicked);
		}
	}
	QObject::connect(ui->pbtn_plcStartCompensate, &QPushButton::clicked,
		this, &EdgeWidthDetection::pbtn_plcStartCompensate_clicked);
	QObject::connect(ui->pbtn_plcAutoRecipe, &QPushButton::clicked,
		this, &EdgeWidthDetection::pbtn_plcAutoRecipe_clicked);

	QObject::connect(&_mainUiRefreshTimer, &QTimer::timeout,
		this, &EdgeWidthDetection::onMainUiRefreshTimeout);
	_mainUiRefreshTimer.setInterval(500);
	_mainUiRefreshTimer.start();

	// 初始按补偿关闭显示，首次轮询后校正为真实状态
	updateCutCompensateButton();
	updateAutoRecipeButton();
}

void EdgeWidthDetection::loadMainUiPoints()
{
	// modbus_main.csv 每行：名称,类型,协议地址,读写
	// 与 DlgModbus 的 modbus.csv 相互独立；名称必须是约定的点位名且各出现一次
	// 严格校验：任何一行格式非法都会弹窗提示并退出程序，防止错误配置静默生效
	// 向后兼容：老版本 CSV 缺少新增点位时按内置默认表自动补齐并回写，无需现场手工改文件
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

	if (!errors.isEmpty())
	{
		_mainUiPoints.clear();
		abortOnMainUiLoadErrors(errors);
	}

	// 缺项补齐：老版本 modbus_main.csv 缺少新增点位（如升级后新增的配方按钮）时，
	// 按内置默认表追加并回写 CSV，避免现场手工改文件
	QStringList missingNames;
	for (const auto& name : kMainUiPointNames)
	{
		if (!seenNames.contains(name))
		{
			missingNames << name;
		}
	}
	if (!missingNames.isEmpty())
	{
		qDebug() << "modbus_main.csv 缺少点位，已按默认表补齐:" << missingNames;
		for (const auto& d : kMainUiDefaults)
		{
			const QString name = QString::fromUtf8(d.name);
			if (missingNames.contains(name))
			{
				MainUiPoint info;
				info.name = name;
				info.type = d.type;
				info.protocolAddress = d.protocolAddress;
				info.writable = d.writable;
				_mainUiPoints.push_back(info);
			}
		}
		saveMainUiPoints();
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
	qDebug() << "[写入线圈] 请求:" << pointName << "| 目标值:" << value;
	if (!checkManualWriteReady())
	{
		qWarning() << "[写入线圈]" << pointName << "被前置检查拦截: manualWriteEnabled ="
			<< Modules::getInstance().plcController.manualWriteEnabled.load()
			<< "| scheduler 存在:" << (Modules::getInstance().plcController.plcControllerScheduler != nullptr);
		return false;
	}

	const auto* point = findMainUiPoint(pointName);
	if (!point || point->type != MainPointType::Bool || !point->writable)
	{
		qWarning() << "[写入线圈]" << pointName << "点位检查失败: 找到点位:" << (point != nullptr)
			<< (point ? QStringLiteral("| 类型:%1 | 可写:%2 | 地址:%3")
				.arg(static_cast<int>(point->type)).arg(point->writable).arg(point->protocolAddress)
				: QString());
		QMessageBox::warning(this, QStringLiteral("警告"),
			QStringLiteral("modbus_main.csv 中未找到可写 BOOL 点位「%1」，请检查点位表").arg(pointName));
		return false;
	}

	auto& scheduler = Modules::getInstance().plcController.plcControllerScheduler;
	const auto addr = static_cast<rw::hoem::Address16>(point->protocolAddress);
	qDebug() << "[写入线圈]" << pointName << "| 协议地址:" << point->protocolAddress << "| 调用 writeCoilAsync";
	const bool ok = scheduler->writeCoilAsync(addr, value).get();
	qDebug() << "[写入线圈]" << pointName << "| writeCoilAsync 返回:" << ok;
	if (!ok)
	{
		QMessageBox::warning(this, QStringLiteral("警告"),
			pointName + (value ? QStringLiteral(" 置1失败") : QStringLiteral(" 置0失败")));
		return false;
	}

	// 回读校验：确认 PLC 侧线圈的实际值。
	// 若写入 1 成功但回读仍为 0，说明 PLC 程序消费后自复位（瞬动设计），属正常现象；
	// 若回读失败而轮询读取正常，则需对比两者地址是否一致
	const auto rb = scheduler->readCoilAsync(addr).get();
	qDebug() << "[写入线圈]" << pointName << "| 回读:"
		<< (rb.second ? QString::number(rb.first) : QStringLiteral("失败"));
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
	// 复制尺寸策略与固定宽高约束，保持布局锁定（数值 label 定宽）在替换后依然生效
	clickable->setSizePolicy(oldLabel->sizePolicy());
	clickable->setMinimumSize(oldLabel->minimumSize());
	clickable->setMaximumSize(oldLabel->maximumSize());
	clickable->setCursor(Qt::PointingHandCursor);
	clickable->setStyleSheet(QString::fromUtf8(kMainClickableValueStyle));
	// replaceWidget 会递归查找子布局（与 ini_clickableTitle 中标题标签的替换方式相同）
	oldLabel->parentWidget()->layout()->replaceWidget(oldLabel, clickable);
	delete oldLabel;
	return clickable;
}

void EdgeWidthDetection::updateCutCompensateButton()
{
	const char* style = _cutCompensateOn ? kCutCompensateOnStyle : kCutCompensateOffStyle;
	// 「开始补偿」按钮文字固定、仅随状态变色
	ui->pbtn_plcStartCompensate->setStyleSheet(QString::fromUtf8(style));
}

void EdgeWidthDetection::updateAutoRecipeButton()
{
	ui->pbtn_plcAutoRecipe->setStyleSheet(QString::fromUtf8(
		_autoRecipeOn ? kCutCompensateOnStyle : kCutCompensateOffStyle));
}

void EdgeWidthDetection::updateRecipeButtons(int recipeNo)
{
	// 12 个配方按钮颜色仅由「配方号」点位驱动：等于按钮值=绿，否则=红（150/160 占位按钮同规则变色，但无点击逻辑）
	// 使用配方按钮专用样式（按下反馈更明显）
	for (int i = 0; i < _recipeButtons.size() && i < kRecipeBtnCount; ++i)
	{
		const bool on = (recipeNo == kRecipeBtnValues[i]);
		_recipeButtons[i]->setStyleSheet(QString::fromUtf8(
			on ? kRecipeBtnOnStyle : kRecipeBtnOffStyle));
	}
}

void EdgeWidthDetection::pbtn_plcStartCompensate_clicked()
{
	// 「开始补偿」与「切刀补偿」是同一 PLC 点位（3000 线圈），取反写入
	writeMainCoil(QStringLiteral("切刀补偿"), !_cutCompensateOn);
	onMainUiRefreshTimeout();
}

void EdgeWidthDetection::pbtn_plcAutoRecipe_clicked()
{
	// 切换开关：按当前状态取反写入；按钮颜色由轮询读到的新状态刷新
	writeMainCoil(QStringLiteral("配方自动选择"), !_autoRecipeOn);
	onMainUiRefreshTimeout();
}

void EdgeWidthDetection::onPlcRecipeBtn_clicked()
{
	auto* btn = qobject_cast<QPushButton*>(sender());
	const int idx = _recipeButtons.indexOf(btn);
	if (idx < 0 || idx >= kRecipeBtnCount)
	{
		qWarning() << "[配方按钮] sender 不在配方按钮列表中，忽略本次点击";
		return;
	}
	// 按下置 1、再按置 0：按该按钮线圈最近一次轮询值取反写入
	const QString name = QStringLiteral("配方%1").arg(kRecipeBtnValues[idx]);
	const bool polled = _mainUiBoolStates.contains(name);	// 该线圈是否至少成功轮询过一次
	const bool lastKnown = _mainUiBoolStates.value(name, false);
	const bool target = !lastKnown;
	qDebug() << "[配方按钮] 点击:" << name << "| 按钮索引:" << idx
		<< "| 线圈轮询成功过:" << polled << "| 最近一次轮询值:" << lastKnown
		<< "| 本次准备写入:" << target;
	writeMainCoil(name, target);
	onMainUiRefreshTimeout();
}

void EdgeWidthDetection::onMainUiRefreshTimeout()
{
	auto& scheduler = Modules::getInstance().plcController.plcControllerScheduler;
	if (!scheduler)
	{
		// PLC 未连接时数值行显示 --，指示灯置灰
		for (const auto& labels : _mainUiValueLabels)
		{
			for (auto* lb : labels)
			{
				lb->setText(QStringLiteral("--"));
			}
		}
		setLampColor(ui->label_plcCutPosLimit, false, false, QString(), QString());
		setLampColor(ui->label_plcCutNegLimit, false, false, QString(), QString());
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

				auto labelsIt = _mainUiValueLabels.constFind(point.name);
				if (labelsIt != _mainUiValueLabels.constEnd())
				{
					for (auto* lb : labelsIt.value())
					{
						lb->setText(res.ok ? res.text : QStringLiteral("--"));
					}
				}
				else if (point.name == QStringLiteral("切刀正限位"))
				{
					// PLC 操作列限位灯按客户图要求：0 绿、1 红
					setLampColor(ui->label_plcCutPosLimit, res.ok, res.boolValue,
						QStringLiteral("red"), QStringLiteral("#00e000"));
				}
				else if (point.name == QStringLiteral("切刀负限位"))
				{
					setLampColor(ui->label_plcCutNegLimit, res.ok, res.boolValue,
						QStringLiteral("red"), QStringLiteral("#00e000"));
				}
				else if (point.name == QStringLiteral("切刀补偿"))
				{
					if (res.ok)
					{
						_cutCompensateOn = res.boolValue;
						updateCutCompensateButton();
					}
				}
				else if (point.name == QStringLiteral("配方自动选择"))
				{
					if (res.ok)
					{
						_autoRecipeOn = res.boolValue;
						updateAutoRecipeButton();
					}
				}
				else if (point.name == QStringLiteral("配方号"))
				{
					// 12 个配方按钮颜色仅由配方号驱动（与该按钮自身线圈无关），读取失败保持不变
					if (res.ok)
					{
						updateRecipeButtons(res.text.toInt());
					}
				}
				else if (point.name.startsWith(QStringLiteral("配方")))
				{
					// 记录各配方按钮线圈当前值，点击按钮时据此取反写入
					if (res.ok)
					{
						_mainUiBoolStates.insert(point.name, res.boolValue);
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

// 自动曝光信息：固定各数值字段宽度，保证每行文本长度恒定，
// 避免曝光值位数变化导致 label_warnningInfo 随之拉伸/缩短、进而牵动侧栏布局
static QString formatExposureInfo(const QString& camName, double currentExposure, double targetExposure,
	double meanIntensity, double overRatio, double underRatio)
{
	return QString("%1 均值:%2 过曝:%3% 欠曝:%4% 当前曝光:%5 目标曝光:%6")
		.arg(camName)
		.arg(meanIntensity, 5, 'f', 1)       // 0.0 ~ 255.0
		.arg(overRatio * 100.0, 5, 'f', 1)   // 0.0 ~ 100.0
		.arg(underRatio * 100.0, 5, 'f', 1)  // 0.0 ~ 100.0
		.arg(currentExposure, 5, 'f', 0)     // 曝光值，收敛后 100 ~ 1000
		.arg(targetExposure, 5, 'f', 0);     // 目标曝光 100 ~ 1000
}

void EdgeWidthDetection::onAutoExposureInfo(double currentExposure, double targetExposure, double meanIntensity,
	double overRatio, double underRatio)
{
	_cam1ExposureInfo = formatExposureInfo(QStringLiteral("相机1"), currentExposure, targetExposure, meanIntensity, overRatio, underRatio);
	refreshExposureInfo();
}

void EdgeWidthDetection::onAutoExposureInfo2(double currentExposure, double targetExposure, double meanIntensity,
	double overRatio, double underRatio)
{
	_cam2ExposureInfo = formatExposureInfo(QStringLiteral("相机2"), currentExposure, targetExposure, meanIntensity, overRatio, underRatio);
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