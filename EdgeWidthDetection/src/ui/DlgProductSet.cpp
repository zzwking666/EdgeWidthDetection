#include "DlgProductSet.h"
#include "ui_DlgProductSet.h"

#include <QMessageBox>
#include <QTabWidget>
#include <QtConcurrent/qtconcurrentrun.h>
#include "Modules.hpp"
#include "NumberKeyboard.h"

DlgProductSet::DlgProductSet(QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::DlgProductSetClass())
{
	ui->setupUi(this);

	build_ui();

	build_connect();
}

DlgProductSet::~DlgProductSet()
{
	delete ui;
}

void DlgProductSet::build_ui()
{
	read_config();
}

void DlgProductSet::read_config()
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;

	// 普通参数
	ui->btn_xiangjiguangdianpingbishijian->setText(QString::number(setConfig.xiangjiguangdianpingbishijian));
	ui->btn_shuchuxinhaoyanshi->setText(QString::number(setConfig.shuchuxinhaoyanshi));
	ui->btn_shuchuxinhaochixushijian->setText(QString::number(setConfig.shuchuxinhaochixushijian));

	// 相机参数
	ui->btn_shangxianwei1->setText(QString::number(setConfig.shangxianwei1));
	ui->btn_xiaxianwei1->setText(QString::number(setConfig.xiaxianwei1));
	ui->btn_zuoxianwei1->setText(QString::number(setConfig.zuoxianwei1));
	ui->btn_youxianwei1->setText(QString::number(setConfig.youxianwei1));
	ui->btn_baoguang1->setText(QString::number(setConfig.baoguang1));
	ui->btn_zengyi1->setText(QString::number(setConfig.zengyi1));
	ui->btn_xiangsudangliang1->setText(QString::number(setConfig.xiangsudangliang1));

	ui->tabWidget->setCurrentIndex(0);
}

void DlgProductSet::build_connect()
{
	connect(ui->btn_close, &QPushButton::clicked, this, &DlgProductSet::btn_close_clicked);
	connect(ui->btn_xiangjiguangdianpingbishijian, &QPushButton::clicked, this, &DlgProductSet::btn_xiangjiguangdianpingbishijian_clicked);
	connect(ui->btn_shuchuxinhaoyanshi, &QPushButton::clicked, this, &DlgProductSet::btn_shuchuxinhaoyanshi_clicked);
	connect(ui->btn_shuchuxinhaochixushijian, &QPushButton::clicked, this, &DlgProductSet::btn_shuchuxinhaochixushijian_clicked);
	connect(ui->btn_testTrigger1_1, &QPushButton::clicked, this, &DlgProductSet::btn_testTrigger1_1_clicked);
	connect(ui->btn_testTrigger2_1, &QPushButton::clicked, this, &DlgProductSet::btn_testTrigger2_1_clicked);
	connect(ui->btn_shangxianwei1, &QPushButton::clicked, this, &DlgProductSet::btn_shangxianwei1_clicked);
	connect(ui->btn_xiaxianwei1, &QPushButton::clicked, this, &DlgProductSet::btn_xiaxianwei1_clicked);
	connect(ui->btn_zuoxianwei1, &QPushButton::clicked, this, &DlgProductSet::btn_zuoxianwei1_clicked);
	connect(ui->btn_youxianwei1, &QPushButton::clicked, this, &DlgProductSet::btn_youxianwei1_clicked);
	connect(ui->btn_baoguang1, &QPushButton::clicked, this, &DlgProductSet::btn_baoguang1_clicked);
	connect(ui->btn_zengyi1, &QPushButton::clicked, this, &DlgProductSet::btn_zengyi1_clicked);
	connect(ui->btn_xiangsudangliang1, &QPushButton::clicked, this, &DlgProductSet::btn_xiangsudangliang1_clicked);
}

void DlgProductSet::btn_close_clicked()
{
	emit paramsChanged();
	this->close();
}

void DlgProductSet::btn_xiangjiguangdianpingbishijian_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于等于0的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_xiangjiguangdianpingbishijian->setText(value);
		setConfig.xiangjiguangdianpingbishijian = value.toDouble();
	}
}

void DlgProductSet::btn_shuchuxinhaoyanshi_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于等于0的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_shuchuxinhaoyanshi->setText(value);
		setConfig.shuchuxinhaoyanshi = value.toDouble();
	}
}

void DlgProductSet::btn_shuchuxinhaochixushijian_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于等于0的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_shuchuxinhaochixushijian->setText(value);
		setConfig.shuchuxinhaochixushijian = value.toDouble();
	}
}

void DlgProductSet::btn_testTrigger1_1_clicked()
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	auto& camera = Modules::getInstance().cameraModule.camera1;
	// 剔废动作
	rw::rqw::OutTriggerConfig outTriggerConfig;
	outTriggerConfig.lineSelector = 1;
	outTriggerConfig.lineMode = 8;
	outTriggerConfig.lineSource = 5;
	outTriggerConfig.durationValue = 400 * 1000;
	outTriggerConfig.strobeEnable = true;
	if (camera)
	{
		camera->setOutTriggerConfig(outTriggerConfig);
		camera->outTrigger();
	}
}

void DlgProductSet::btn_testTrigger2_1_clicked()
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	auto& camera = Modules::getInstance().cameraModule.camera1;
	// 剔废动作
	rw::rqw::OutTriggerConfig outTriggerConfig;
	outTriggerConfig.lineSelector = 2;
	outTriggerConfig.lineMode = 8;
	outTriggerConfig.lineSource = 5;
	outTriggerConfig.durationValue = 400 * 1000;
	outTriggerConfig.strobeEnable = true;
	if (camera)
	{
		camera->setOutTriggerConfig(outTriggerConfig);
		camera->outTrigger();
	}
}

void DlgProductSet::btn_shangxianwei1_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于等于0的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_shangxianwei1->setText(value);
		setConfig.shangxianwei1 = value.toDouble();
	}
}

void DlgProductSet::btn_xiaxianwei1_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于等于0的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_xiaxianwei1->setText(value);
		setConfig.xiaxianwei1 = value.toDouble();
	}
}

void DlgProductSet::btn_zuoxianwei1_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于等于0的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_zuoxianwei1->setText(value);
		setConfig.zuoxianwei1 = value.toDouble();
	}
}

void DlgProductSet::btn_youxianwei1_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于等于0的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_youxianwei1->setText(value);
		setConfig.youxianwei1 = value.toDouble();
	}
}

void DlgProductSet::btn_baoguang1_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于等于0的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		auto& camera = Modules::getInstance().cameraModule.camera1;
		if (camera)
		{
			camera->setExposureTime(value.toInt());
		}
		ui->btn_baoguang1->setText(value);
		setConfig.baoguang1 = value.toDouble();
	}
}

void DlgProductSet::btn_zengyi1_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于等于0的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		auto& camera = Modules::getInstance().cameraModule.camera1;
		if (camera)
		{
			camera->setGain(value.toInt());
		}
		ui->btn_zengyi1->setText(value);
		setConfig.zengyi1 = value.toDouble();
	}
}

void DlgProductSet::btn_xiangsudangliang1_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() <= 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于0的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_xiangsudangliang1->setText(value);
		setConfig.xiangsudangliang1 = value.toDouble();
	}
}