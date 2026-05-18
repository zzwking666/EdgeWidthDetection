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
	ui->cbox_changeSaveImgMode->setCurrentIndex(setConfig.saveImgMode);
	ui->btn_score->setText(QString::number(setConfig.score));
	ui->ckb_autoSaveImg->setChecked(setConfig.autoSaveImg);

	// 相机参数
	ui->btn_shangxianwei1->setText(QString::number(setConfig.shangxianwei1));
	ui->btn_xiaxianwei1->setText(QString::number(setConfig.xiaxianwei1));
	ui->btn_zuoxianwei1->setText(QString::number(setConfig.zuoxianwei1));
	ui->btn_youxianwei1->setText(QString::number(setConfig.youxianwei1));
	ui->btn_xiangsudangliang1->setText(QString::number(setConfig.xiangsudangliang1));
	ui->btn_debugFrame->setText(QString::number(setConfig.debugFrame));
	ui->btn_openRemoveFrame->setText(QString::number(setConfig.openRemoveFrame));

	// 亮度调整 
	ui->btn_zengyi->setText(QString::number(setConfig.zengyi));
	ui->btn_qiangguang->setText(QString::number(setConfig.qiangguang));
	ui->btn_zhongguang->setText(QString::number(setConfig.zhongguang));
	ui->btn_ruoguang->setText(QString::number(setConfig.ruoguang));

	// modbus地址
	ui->cbox_registerAttribute->setCurrentIndex(setConfig.registerAttribute);
	ui->btn_paizhaowanchengxinhaodizhi->setText(QString::number(setConfig.paizhaowanchengxinhaodizhi));
	ui->btn_kuanduxierudizhi->setText(QString::number(setConfig.kuanduxierudizhi));

	ui->tabWidget->setCurrentIndex(0);
}

void DlgProductSet::build_connect()
{
	connect(ui->btn_close, &QPushButton::clicked, this, &DlgProductSet::btn_close_clicked);
	connect(ui->btn_xiangjiguangdianpingbishijian, &QPushButton::clicked, this, &DlgProductSet::btn_xiangjiguangdianpingbishijian_clicked);
	connect(ui->btn_shuchuxinhaoyanshi, &QPushButton::clicked, this, &DlgProductSet::btn_shuchuxinhaoyanshi_clicked);
	connect(ui->btn_shuchuxinhaochixushijian, &QPushButton::clicked, this, &DlgProductSet::btn_shuchuxinhaochixushijian_clicked);
	connect(ui->cbox_changeSaveImgMode, &QComboBox::currentIndexChanged, this, &DlgProductSet::cbox_changeSaveImgMode_clicked);
	connect(ui->btn_score, &QPushButton::clicked, this, &DlgProductSet::btn_score_clicked);
	connect(ui->btn_testTrigger1_1, &QPushButton::clicked, this, &DlgProductSet::btn_testTrigger1_1_clicked);
	connect(ui->btn_testTrigger2_1, &QPushButton::clicked, this, &DlgProductSet::btn_testTrigger2_1_clicked);
	connect(ui->btn_shangxianwei1, &QPushButton::clicked, this, &DlgProductSet::btn_shangxianwei1_clicked);
	connect(ui->btn_xiaxianwei1, &QPushButton::clicked, this, &DlgProductSet::btn_xiaxianwei1_clicked);
	connect(ui->btn_zuoxianwei1, &QPushButton::clicked, this, &DlgProductSet::btn_zuoxianwei1_clicked);
	connect(ui->btn_youxianwei1, &QPushButton::clicked, this, &DlgProductSet::btn_youxianwei1_clicked);
	connect(ui->btn_zengyi, &QPushButton::clicked, this, &DlgProductSet::btn_zengyi_clicked);
	connect(ui->btn_xiangsudangliang1, &QPushButton::clicked, this, &DlgProductSet::btn_xiangsudangliang1_clicked);
	connect(ui->btn_debugFrame, &QPushButton::clicked, this, &DlgProductSet::btn_debugFrame_clicked);
	connect(ui->btn_openRemoveFrame, &QPushButton::clicked, this, &DlgProductSet::btn_openRemoveFrame_clicked);
	connect(ui->btn_qiangguang, &QPushButton::clicked, this, &DlgProductSet::btn_qiangguang_clicked);
	connect(ui->btn_zhongguang, &QPushButton::clicked, this, &DlgProductSet::btn_zhongguang_clicked);
	connect(ui->btn_ruoguang, &QPushButton::clicked, this, &DlgProductSet::btn_ruoguang_clicked);
	connect(ui->cbox_registerAttribute, &QComboBox::currentIndexChanged, this, &DlgProductSet::cbox_registerAttribute_clicked);
	connect(ui->btn_paizhaowanchengxinhaodizhi, &QPushButton::clicked, this, &DlgProductSet::btn_paizhaowanchengxinhaodizhi_clicked);
	connect(ui->btn_kuanduxierudizhi, &QPushButton::clicked, this, &DlgProductSet::btn_kuanduxierudizhi_clicked);
	connect(ui->ckb_autoSaveImg, &QCheckBox::clicked, this, &DlgProductSet::ckb_autoSaveImg_clicked);
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

void DlgProductSet::cbox_changeSaveImgMode_clicked()
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	setConfig.saveImgMode = ui->cbox_changeSaveImgMode->currentIndex();
}

void DlgProductSet::btn_score_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0 || value.toDouble() > 100)
		{
			QMessageBox::warning(this, "提示", "请输入0到100之间的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_score->setText(value);
		setConfig.score = value.toInt();
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

void DlgProductSet::btn_zengyi_clicked()
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
		ui->btn_zengyi->setText(value);
		setConfig.zengyi = value.toDouble();
	}
}

void DlgProductSet::btn_qiangguang_clicked()
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
		ui->btn_qiangguang->setText(value);
		setConfig.qiangguang = value.toDouble();
	}
}

void DlgProductSet::btn_zhongguang_clicked()
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
		ui->btn_zhongguang->setText(value);
		setConfig.zhongguang = value.toDouble();
	}
}

void DlgProductSet::btn_ruoguang_clicked()
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
		ui->btn_ruoguang->setText(value);
		setConfig.ruoguang = value.toDouble();
	}
}

void DlgProductSet::cbox_registerAttribute_clicked()
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	setConfig.registerAttribute = ui->cbox_registerAttribute->currentIndex();
}

void DlgProductSet::btn_paizhaowanchengxinhaodizhi_clicked()
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
		ui->btn_paizhaowanchengxinhaodizhi->setText(value);
		setConfig.paizhaowanchengxinhaodizhi = value.toInt();
	}
}

void DlgProductSet::btn_kuanduxierudizhi_clicked()
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
		ui->btn_kuanduxierudizhi->setText(value);
		setConfig.kuanduxierudizhi = value.toInt();
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

void DlgProductSet::btn_debugFrame_clicked()
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
		ui->btn_debugFrame->setText(value);
		setConfig.debugFrame = value.toDouble();
		auto& runningState = Modules::getInstance().runtimeInfoModule.runningState;
		if (runningState == RunningState::Debug)
		{
			auto& camera = Modules::getInstance().cameraModule.camera1;
			if (camera)
			{
				camera->setFrameRate(setConfig.debugFrame);
			}
		}
	}
}

void DlgProductSet::btn_openRemoveFrame_clicked()
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
		ui->btn_openRemoveFrame->setText(value);
		setConfig.openRemoveFrame = value.toDouble();
		auto& runningState = Modules::getInstance().runtimeInfoModule.runningState;
		if (runningState == RunningState::OpenRemoveFunc)
		{
			auto& camera = Modules::getInstance().cameraModule.camera1;
			if (camera)
			{
				camera->setFrameRate(setConfig.openRemoveFrame);
			}
		}
	}
}

void DlgProductSet::ckb_autoSaveImg_clicked()
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	setConfig.autoSaveImg = ui->ckb_autoSaveImg->isChecked();
}
