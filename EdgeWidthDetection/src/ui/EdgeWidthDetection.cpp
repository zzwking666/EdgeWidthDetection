#include "EdgeWidthDetection.h"

#include <QDir>
#include <QMessageBox>
#include <QProcess>

#include "ui_EdgeWidthDetection.h"
#include <QPushButton>

#include "DlgProductSet.h"
#include "Modules.hpp"
#include "NumberKeyboard.h"
#include "rqw_RunEnvCheck.hpp"
#include "Utilty.hpp"


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
	QObject::connect(ui->rbtn_debug, &QRadioButton::clicked,
		this, &EdgeWidthDetection::rbtn_debug_checked);
	QObject::connect(ui->rbtn_removeFunc, &QRadioButton::clicked,
		this, &EdgeWidthDetection::rbtn_removeFunc_checked);
	QObject::connect(ui->pbtn_resetProduct, &QPushButton::clicked,
		this, &EdgeWidthDetection::pbtn_resetProduct_clicked);
	QObject::connect(ui->ckb_saveImg, &QCheckBox::clicked,
		this, &EdgeWidthDetection::ckb_saveImg_checked);
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
	maiLiDingZiConfig.isSaveImg = false;	// 默认不开启图片保存
	ui->ckb_saveImg->setChecked(false);

	ui->label_wasteProductsValue->setText(QString::number(maiLiDingZiConfig.totalDefectiveVolume));
	ui->rbtn_removeFunc->setChecked(maiLiDingZiConfig.isDefect);

	rbtn_removeFunc_checked(true);

	ini_clickableTitle();
}

void EdgeWidthDetection::ini_clickableTitle()
{
	// 初始化标题label
	clickableTitle = new rw::rqw::ClickableLabel(this);
	auto layoutTitle = ui->groupBox_head->layout();
	layoutTitle->replaceWidget(ui->label_title, clickableTitle);
	delete ui->label_title;
	clickableTitle->setText("印刷检测");
	clickableTitle->setStyleSheet("QLabel {font-size: 30px;font-weight: bold;color: rgb(255, 255, 255);padding: 5px 5px;border-bottom: 2px solid #cccccc;}");
}

void EdgeWidthDetection::build_DlgCloseForm()
{
	_dlgCloseForm = new DlgCloseForm(this);
}

void EdgeWidthDetection::initializeComponents()
{
	build_ui();

	build_camera();

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

void EdgeWidthDetection::updateCameraLabelState(int cameraIndex, bool state)
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	switch (cameraIndex)
	{
	case 1:
		if (state) {
			ui->label_camera1State->setText("连接成功");
			ui->label_camera1State->setStyleSheet(QString("QLabel{color:rgb(0, 230, 0);font-size: 18px;font - weight: bold;padding: 5px 5px;} "));
		}
		else {
			ui->label_camera1State->setText("连接失败");
			ui->label_camera1State->setStyleSheet(QString("QLabel{color:rgb(230, 0, 0);font-size: 18px;font - weight: bold;padding: 5px 5px;} "));
		}
		break;
	case 2:
		if (state) {
			ui->label_camera2State->setText("连接成功");
			ui->label_camera2State->setStyleSheet(QString("QLabel{color:rgb(0, 230, 0);font-size: 18px;font - weight: bold;padding: 5px 5px;} "));
		}
		else {
			ui->label_camera2State->setText("连接失败");
			ui->label_camera2State->setStyleSheet(QString("QLabel{color:rgb(230, 0, 0);font-size: 18px;font - weight: bold;padding: 5px 5px;} "));
		}
		break;
	default:
		break;
	}
}

void EdgeWidthDetection::onUpdateStatisticalInfoUI()
{
	auto& statisticalInfo = Modules::getInstance().runtimeInfoModule.statisticalInfo;
	ui->label_wasteProductsValue->setText(QString::number(statisticalInfo.wasteCount.load()));
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

void EdgeWidthDetection::rbtn_debug_checked(bool checked)
{
	auto isRuning = ui->rbtn_removeFunc->isChecked();

	auto& runningState = Modules::getInstance().runtimeInfoModule.runningState;
	auto& camera1 = Modules::getInstance().cameraModule.camera1;
	if (!isRuning) {
		if (checked) {
			runningState = RunningState::Debug;
			if (camera1)
			{
				camera1->setTriggerState(false);
				camera1->setFrameRate(5);
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
	if (checked)
	{
		runningState = RunningState::OpenRemoveFunc;
		if (camera1)
		{
			camera1->setTriggerState(true);
			camera1->setTriggerSource(rw::rqw::TriggerSource::Line0);
			camera1->setFrameRate(50);
		}
		ui->rbtn_debug->setChecked(false);
	}
	else
	{
		runningState = RunningState::Stop;
	}
}

void EdgeWidthDetection::pbtn_resetProduct_clicked()
{
	auto& maiLiDingZiConfig = Modules::getInstance().configManagerModule.edgeWidthDetectionConfig;

	maiLiDingZiConfig.totalDefectiveVolume = 0;

	onUpdateStatisticalInfoUI();
}

void EdgeWidthDetection::ckb_saveImg_checked(bool checked)
{
	auto& maiLiDingZiConfig = Modules::getInstance().configManagerModule.edgeWidthDetectionConfig;
	maiLiDingZiConfig.isSaveImg = checked;
}
