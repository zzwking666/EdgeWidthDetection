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
	ui->btn_shicekuanduduqudizhi->setText(QString::number(setConfig.shicekuanduduqudizhi));
	ui->btn_shedingbiaozhunzhiduqudizhi->setText(QString::number(setConfig.shedingbiaozhunzhiduqudizhi));
	ui->btn_changdujiangeduqudizhi->setText(QString::number(setConfig.changdujiangeduqudizhi));
	ui->btn_paizhaochangdujiangeduqudizhi->setText(QString::number(setConfig.paizhaochangdujiangeduqudizhi));
	ui->btn_bujinyiquanmaichongshuduqudizhi->setText(QString::number(setConfig.bujinyiquanmaichongshuduqudizhi));
	ui->btn_luojuduqudizhi->setText(QString::number(setConfig.luojuduqudizhi));
	ui->btn_huodePLCbaojingxinxidizhi->setText(QString::number(setConfig.huodePLCbaojingxinxidizhi));

	ui->tabWidget->setCurrentIndex(0);
	ui->tabWidget_PLC->setCurrentIndex(0);
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
	
	// 读取PLC
	connect(ui->btn_shicekuanduduqudizhi, &QPushButton::clicked, this, &DlgProductSet::btn_shicekuanduduqudizhi_clicked);
	connect(ui->btn_shedingbiaozhunzhiduqudizhi, &QPushButton::clicked, this, &DlgProductSet::btn_shedingbiaozhunzhiduqudizhi_clicked);
	connect(ui->btn_changdujiangeduqudizhi, &QPushButton::clicked, this, &DlgProductSet::btn_changdujiangeduqudizhi_clicked);
	connect(ui->btn_paizhaochangdujiangeduqudizhi, &QPushButton::clicked, this, &DlgProductSet::btn_paizhaochangdujiangeduqudizhi_clicked);
	connect(ui->btn_bujinyiquanmaichongshuduqudizhi, &QPushButton::clicked, this, &DlgProductSet::btn_bujinyiquanmaichongshuduqudizhi_clicked);
	connect(ui->btn_luojuduqudizhi, &QPushButton::clicked, this, &DlgProductSet::btn_luojuduqudizhi_clicked);
	connect(ui->btn_huodePLCbaojingxinxidizhi, &QPushButton::clicked, this, &DlgProductSet::btn_huodePLCbaojingxinxidizhi_clicked);
	// 写入PLC
	connect(ui->btn_shicekuanduxierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_shicekuanduxierushuzhi_clicked);
	connect(ui->btn_shedingbiaozhunzhixierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_shedingbiaozhunzhixierushuzhi_clicked);
	connect(ui->btn_changdujiangexierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_changdujiangexierushuzhi_clicked);
	connect(ui->btn_paizhaochangdujiangexierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_paizhaochangdujiangexierushuzhi_clicked);
	connect(ui->btn_bujinyiquanmaichongshuxierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_bujinyiquanmaichongshuxierushuzhi_clicked);
	connect(ui->btn_luojuxierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_luojuxierushuzhi_clicked);

	connect(ui->btn_writeshicekuandu, &QPushButton::clicked, this, &DlgProductSet::btn_writeshicekuandu_clicked);
	connect(ui->btn_writeshedingbiaozhunzhi, &QPushButton::clicked, this, &DlgProductSet::btn_writeshedingbiaozhunzhi_clicked);
	connect(ui->btn_writechangdujiange, &QPushButton::clicked, this, &DlgProductSet::btn_writechangdujiange_clicked);
	connect(ui->btn_writepaizhaochangdujiange, &QPushButton::clicked, this, &DlgProductSet::btn_writepaizhaochangdujiange_clicked);
	connect(ui->btn_writebujinyiquanmaichongshu, &QPushButton::clicked, this, &DlgProductSet::btn_writebujinyiquanmaichongshu_clicked);
	connect(ui->btn_writeluojuxierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_writeluojuxierushuzhi_clicked);

	connect(ui->ckb_autoSaveImg, &QCheckBox::clicked, this, &DlgProductSet::ckb_autoSaveImg_clicked);
	connect(ui->tabWidget, &QTabWidget::currentChanged, this, &DlgProductSet::tabWidget_indexChanged);
}

void DlgProductSet::onUpdatePLCInfo(QVector<PlcReadItem> datas)
{
	auto applyValue = [](QPushButton* btn, const PlcReadItem& item)
		{
			if (item.ok)
			{
				btn->setText(QString::number(item.value));
			}
			else
			{
				btn->setText("读取失败");
			}
		};

	for (const auto& item : datas)
	{
		switch (item.index)
		{
		case PlcReadIndex::shicekuandu:
			applyValue(ui->btn_shicekuanduduqushuzhi, item);
			break;
		case PlcReadIndex::shedingbiaozhunzhi:
			applyValue(ui->btn_shedingbiaozhunzhiduqushuzhi, item);
			break;
		case PlcReadIndex::changdujiange:
			applyValue(ui->btn_changdujiangeduqushuzhi, item);
			break;
		case PlcReadIndex::paizhaochangdujiange:
			applyValue(ui->btn_paizhaochangdujiangeduqushuzhi, item);
			break;
		case PlcReadIndex::bujinyiquanmaichongshu:
			applyValue(ui->btn_bujinyiquanmaichongshuduqushuzhi, item);
			break;
		case PlcReadIndex::luoju:
			applyValue(ui->btn_luojuduqushuzhi, item);
			break;
		default:
			break;
		}
	}
}

void DlgProductSet::btn_close_clicked()
{
	emit paramsChanged();

	auto& plcListenThread = Modules::getInstance().plcController.plcListenThread;
	if (plcListenThread)
	{
		plcListenThread->stopThread();
	}

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

void DlgProductSet::btn_shicekuanduduqudizhi_clicked()
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

		const int newAddress = value.toInt();
		if (!checkIsPLCAddressSame(newAddress, "shice"))
		{
			return;
		}

		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_shicekuanduduqudizhi->setText(value);
		setConfig.shicekuanduduqudizhi = value.toInt();
		ModBusAddress::shicekuanduAddress = value.toInt();
	}
}

void DlgProductSet::btn_shedingbiaozhunzhiduqudizhi_clicked()
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

		const int newAddress = value.toInt();
		if (!checkIsPLCAddressSame(newAddress, "biaozhun"))
		{
			return;
		}

		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_shedingbiaozhunzhiduqudizhi->setText(value);
		setConfig.shedingbiaozhunzhiduqudizhi = value.toInt();
		ModBusAddress::shedingbiaozhunzhiAddress = value.toInt();
	}
}

void DlgProductSet::btn_changdujiangeduqudizhi_clicked()
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

		const int newAddress = value.toInt();
		if (!checkIsPLCAddressSame(newAddress, "changdu"))
		{
			return;
		}


		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_changdujiangeduqudizhi->setText(value);
		setConfig.changdujiangeduqudizhi = value.toInt();
		ModBusAddress::changdujiangeAddress = value.toInt();
	}
}

void DlgProductSet::btn_paizhaochangdujiangeduqudizhi_clicked()
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

		const int newAddress = value.toInt();
		if (!checkIsPLCAddressSame(newAddress, "paizhao"))
		{
			return;
		}

		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_paizhaochangdujiangeduqudizhi->setText(value);
		setConfig.paizhaochangdujiangeduqudizhi = value.toInt();
		ModBusAddress::paizhaochangdujiangeAddress = value.toInt();
	}
}

void DlgProductSet::btn_bujinyiquanmaichongshuduqudizhi_clicked()
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

		const int newAddress = value.toInt();
		if (!checkIsPLCAddressSame(newAddress, "maichong"))
		{
			return;
		}

		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_bujinyiquanmaichongshuduqudizhi->setText(value);
		setConfig.bujinyiquanmaichongshuduqudizhi = value.toInt();
		ModBusAddress::bujinyiquanmaichongshuAddress = value.toInt();
	}
}

void DlgProductSet::btn_luojuduqudizhi_clicked()
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

		const int newAddress = value.toInt();
		if (!checkIsPLCAddressSame(newAddress, "luoju"))
		{
			return;
		}

		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_luojuduqudizhi->setText(value);
		setConfig.luojuduqudizhi = value.toInt();
		ModBusAddress::luojuAddress = value.toInt();
	}
}

void DlgProductSet::btn_huodePLCbaojingxinxidizhi_clicked()
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

		const int newAddress = value.toInt();
		if (!checkIsPLCAddressSame(newAddress, "gaojing"))
		{
			return;
		}

		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_huodePLCbaojingxinxidizhi->setText(value);
		setConfig.huodePLCbaojingxinxidizhi = value.toInt();
		ModBusAddress::readPLCbaojingxinxiAddress = value.toInt();
	}
}

void DlgProductSet::btn_shicekuanduxierushuzhi_clicked()
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
		ui->btn_shicekuanduxierushuzhi->setText(value);
	}
}

void DlgProductSet::btn_shedingbiaozhunzhixierushuzhi_clicked()
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
		ui->btn_shedingbiaozhunzhixierushuzhi->setText(value);
	}
}

void DlgProductSet::btn_changdujiangexierushuzhi_clicked()
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
		ui->btn_changdujiangexierushuzhi->setText(value);
	}
}

void DlgProductSet::btn_paizhaochangdujiangexierushuzhi_clicked()
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
		ui->btn_paizhaochangdujiangexierushuzhi->setText(value);
	}
}

void DlgProductSet::btn_bujinyiquanmaichongshuxierushuzhi_clicked()
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
		ui->btn_bujinyiquanmaichongshuxierushuzhi->setText(value);
	}
}

void DlgProductSet::btn_luojuxierushuzhi_clicked()
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
		ui->btn_luojuxierushuzhi->setText(value);
	}
}

void DlgProductSet::btn_writeshicekuandu_clicked()
{
	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;

	if (plcControllerScheduler)
	{
		uint16_t writeData = ui->btn_shicekuanduxierushuzhi->text().toUInt();
		auto fut = plcControllerScheduler->writeUInt16RegisterAsync(ModBusAddress::shicekuanduAddress, writeData);

		if (fut.get())
		{
			QMessageBox::information(this,"提示", "实测宽度写入成功");
		}
		else
		{
			QMessageBox::information(this, "警告", "实测宽度写入失败");
		}
	}
}

void DlgProductSet::btn_writeshedingbiaozhunzhi_clicked()
{
	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;

	if (plcControllerScheduler)
	{
		uint16_t writeData = ui->btn_shedingbiaozhunzhixierushuzhi->text().toUInt();
		auto fut = plcControllerScheduler->writeUInt16RegisterAsync(ModBusAddress::shedingbiaozhunzhiAddress, writeData);

		if (fut.get())
		{
			QMessageBox::information(this, "提示", "设定标准值写入成功");
		}
		else
		{
			QMessageBox::information(this, "警告", "设定标准值写入失败");
		}
	}
}

void DlgProductSet::btn_writechangdujiange_clicked()
{
	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;

	if (plcControllerScheduler)
	{
		uint16_t writeData = ui->btn_changdujiangexierushuzhi->text().toUInt();
		auto fut = plcControllerScheduler->writeUInt16RegisterAsync(ModBusAddress::changdujiangeAddress, writeData);

		if (fut.get())
		{
			QMessageBox::information(this, "提示", "长度间隔写入成功");
		}
		else
		{
			QMessageBox::information(this, "警告", "长度间隔写入失败");
		}
	}
}

void DlgProductSet::btn_writepaizhaochangdujiange_clicked()
{
	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;

	if (plcControllerScheduler)
	{
		uint16_t writeData = ui->btn_paizhaochangdujiangexierushuzhi->text().toUInt();
		auto fut = plcControllerScheduler->writeUInt16RegisterAsync(ModBusAddress::paizhaochangdujiangeAddress, writeData);

		if (fut.get())
		{
			QMessageBox::information(this, "提示", "拍照长度间隔写入成功");
		}
		else
		{
			QMessageBox::information(this, "警告", "拍照长度间隔写入失败");
		}
	}
}

void DlgProductSet::btn_writebujinyiquanmaichongshu_clicked()
{
	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;

	if (plcControllerScheduler)
	{
		uint16_t writeData = ui->btn_bujinyiquanmaichongshuxierushuzhi->text().toUInt();
		auto fut = plcControllerScheduler->writeUInt16RegisterAsync(ModBusAddress::bujinyiquanmaichongshuAddress, writeData);

		if (fut.get())
		{
			QMessageBox::information(this, "提示", "步进一圈脉冲数写入成功");
		}
		else
		{
			QMessageBox::information(this, "警告", "步进一圈脉冲数写入失败");
		}
	}
}

void DlgProductSet::btn_writeluojuxierushuzhi_clicked()
{
	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;

	if (plcControllerScheduler)
	{
		uint16_t writeData = ui->btn_luojuxierushuzhi->text().toUInt();
		auto fut = plcControllerScheduler->writeUInt16RegisterAsync(ModBusAddress::luojuAddress, writeData);

		if (fut.get())
		{
			QMessageBox::information(this, "提示", "螺距写入成功");
		}
		else
		{
			QMessageBox::information(this, "警告", "螺距写入失败");
		}
	}
}

void DlgProductSet::tabWidget_indexChanged(int index)
{
	if (2 == index)
	{
		auto& plcListenThread = Modules::getInstance().plcController.plcListenThread;
		if (plcListenThread)
		{
			plcListenThread->startThread();
		}
	}
	else
	{
		auto& plcListenThread = Modules::getInstance().plcController.plcListenThread;
		if (plcListenThread)
		{
			plcListenThread->stopThread();
		}
	}
}

bool DlgProductSet::checkIsPLCAddressSame(int newAddress, const QString& currentKey)
{
	const auto& setConfig = Modules::getInstance().configManagerModule.setConfig;

	struct AddrItem
	{
		QString key;
		QString name;
		int value;
	};

	const AddrItem items[] = {
		{ "shice",      "实测宽度读取地址",         setConfig.shicekuanduduqudizhi },
		{ "biaozhun",   "设定标准值读取地址",       setConfig.shedingbiaozhunzhiduqudizhi },
		{ "changdu",    "长度间隔读取地址",         setConfig.changdujiangeduqudizhi },
		{ "paizhao",    "拍照长度间隔读取地址",     setConfig.paizhaochangdujiangeduqudizhi },
		{ "maichong",   "步进一圈脉冲数读取地址",   setConfig.bujinyiquanmaichongshuduqudizhi },
		{ "luoju",      "螺距读取地址",             setConfig.luojuduqudizhi },
		{ "baojing",    "获取PLC报警信息地址",      setConfig.huodePLCbaojingxinxidizhi }
	};

	for (const auto& item : items)
	{
		if (item.key == currentKey)
		{
			continue; // 跳过当前正在设置的这一项
		}
		if (item.value == newAddress)
		{
			QMessageBox::warning(this, "提示",
				QString("地址重复：与“%1”冲突（地址=%2），请重新设置。")
				.arg(item.name).arg(newAddress));
			return false;
		}
	}

	return true;
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
