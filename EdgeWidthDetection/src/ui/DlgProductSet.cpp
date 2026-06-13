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
	ui->ckb_shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan->setChecked(setConfig.shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan);

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
	ui->btn_shiceyahenkuanduduqudizhi->setText(QString::number(setConfig.shiceyahenkuanduduqudizhi));
	ui->btn_shedingyahenbiaozhunzhiduqudizhi->setText(QString::number(setConfig.shedingyahenbiaozhunzhiduqudizhi));
	ui->btn_paizhaojiangejuliduqudizhi->setText(QString::number(setConfig.paizhaojiangejuliduqudizhi));
	ui->btn_daizichangdubiaozhunduqudizhi->setText(QString::number(setConfig.daizichangdubiaozhunduqudizhi));
	ui->btn_jimiqiyiquanmaichongshuduqudizhi->setText(QString::number(setConfig.jimiqiyiquanmaichongshuduqudizhi));
	ui->btn_jimiqiyiquanzhouchangduqudizhi->setText(QString::number(setConfig.jimiqiyiquanzhouchangduqudizhi));
	ui->btn_bujinyiquanmaichongshuduqudizhi->setText(QString::number(setConfig.bujinyiquanmaichongshuduqudizhi));

	ui->btn_bujinluojuduqudizhi->setText(QString::number(setConfig.bujinluojuduqudizhi));
	ui->btn_daoyidongbuchangduqudizhi->setText(QString::number(setConfig.daoyidongbuchangduqudizhi));
	ui->btn_zidongdaoyidongzuidajuliduqudizhi->setText(QString::number(setConfig.zidongdaoyidongzuidajuliduqudizhi));
	ui->btn_daojiakeyidongdezuidajuliduqudizhi->setText(QString::number(setConfig.daojiakeyidongdezuidajuliduqudizhi));
	ui->btn_daoyidongsudumaichongpinlvduqudizhi->setText(QString::number(setConfig.daoyidongsudumaichongpinlvduqudizhi));

	ui->btn_huodePLCbaojingxinxidizhi->setText(QString::number(setConfig.huodePLCbaojingxinxidizhi));
	ui->btn_keyipaizhaoxinhaodizhi->setText(QString::number(setConfig.keyipaizhaoxinhaodizhi));
	ui->btn_tingzhipaizhaoxinhaodizhi->setText(QString::number(setConfig.tingzhipaizhaoxinhaodizhi));
	ui->btn_daizishicechangdudizhi->setText(QString::number(setConfig.daizishicechangdudizhi));
	ui->btn_shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhi->setText(QString::number(setConfig.shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhi));

	// 同步更新 ModBusAddress 静态变量
	ModBusAddress::shiceyahenkuanduAddress = setConfig.shiceyahenkuanduduqudizhi;
	ModBusAddress::shedingyahenbiaozhunkuanduzhiAddress = setConfig.shedingyahenbiaozhunzhiduqudizhi;
	ModBusAddress::paizhaojiangejuliAddress = setConfig.paizhaojiangejuliduqudizhi;
	ModBusAddress::daizichangdubiaozhunAddress = setConfig.daizichangdubiaozhunduqudizhi;
	ModBusAddress::jimiqiyiquanmaichongshuAddress = setConfig.jimiqiyiquanmaichongshuduqudizhi;
	ModBusAddress::jimiqiyiquanzhouchangAddress = setConfig.jimiqiyiquanzhouchangduqudizhi;
	ModBusAddress::bujinyiquanmaichongshuAddress = setConfig.bujinyiquanmaichongshuduqudizhi;
	ModBusAddress::bujinluojuAddress = setConfig.bujinluojuduqudizhi;
	ModBusAddress::daoyidongbuchangAddress = setConfig.daoyidongbuchangduqudizhi;
	ModBusAddress::zidongdaoyidongzuidajuliAddress = setConfig.zidongdaoyidongzuidajuliduqudizhi;
	ModBusAddress::daojiakeyidongdezuidajuliAddress = setConfig.daojiakeyidongdezuidajuliduqudizhi;
	ModBusAddress::daoyidongsudumaichongpinlvAddress = setConfig.daoyidongsudumaichongpinlvduqudizhi;
	ModBusAddress::shibiezhongxindianyutuxiangzhongxindianchazhiAddress = setConfig.shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhi;
	ModBusAddress::readPLCbaojingxinxiAddress = setConfig.huodePLCbaojingxinxidizhi;
	ModBusAddress::readPLCkeyipaizhaoxinhaoAddress = setConfig.keyipaizhaoxinhaodizhi;
	ModBusAddress::readPLCtingzhipaizhaoxinhaoAddress = setConfig.tingzhipaizhaoxinhaodizhi;
	ModBusAddress::readPLCdaizishicechangduAddress = setConfig.daizishicechangdudizhi;

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

	// modbus地址
	connect(ui->btn_shiceyahenkuanduduqudizhi, &QPushButton::clicked, this, &DlgProductSet::btn_shiceyahenkuanduduqudizhi_clicked);
	connect(ui->btn_shedingyahenbiaozhunzhiduqudizhi, &QPushButton::clicked, this, &DlgProductSet::btn_shedingyahenbiaozhunzhiduqudizhi_clicked);
	connect(ui->btn_paizhaojiangejuliduqudizhi, &QPushButton::clicked, this, &DlgProductSet::btn_paizhaojiangejuliduqudizhi_clicked);
	connect(ui->btn_daizichangdubiaozhunduqudizhi, &QPushButton::clicked, this, &DlgProductSet::btn_daizichangdubiaozhunduqudizhi_clicked);
	connect(ui->btn_jimiqiyiquanmaichongshuduqudizhi, &QPushButton::clicked, this, &DlgProductSet::btn_jimiqiyiquanmaichongshuduqudizhi_clicked);
	connect(ui->btn_jimiqiyiquanzhouchangduqudizhi, &QPushButton::clicked, this, &DlgProductSet::btn_jimiqiyiquanzhouchangduqudizhi_clicked);
	connect(ui->btn_bujinyiquanmaichongshuduqudizhi, &QPushButton::clicked, this, &DlgProductSet::btn_bujinyiquanmaichongshuduqudizhi_clicked);

	connect(ui->btn_bujinluojuduqudizhi, &QPushButton::clicked, this, &DlgProductSet::btn_bujinluojuduqudizhi_clicked);
	connect(ui->btn_daoyidongbuchangduqudizhi, &QPushButton::clicked, this, &DlgProductSet::btn_daoyidongbuchangduqudizhi_clicked);
	connect(ui->btn_zidongdaoyidongzuidajuliduqudizhi, &QPushButton::clicked, this, &DlgProductSet::btn_zidongdaoyidongzuidajuliduqudizhi_clicked);
	connect(ui->btn_daojiakeyidongdezuidajuliduqudizhi, &QPushButton::clicked, this, &DlgProductSet::btn_daojiakeyidongdezuidajuliduqudizhi_clicked);
	connect(ui->btn_daoyidongsudumaichongpinlvduqudizhi, &QPushButton::clicked, this, &DlgProductSet::btn_daoyidongsudumaichongpinlvduqudizhi_clicked);

	connect(ui->btn_huodePLCbaojingxinxidizhi, &QPushButton::clicked, this, &DlgProductSet::btn_huodePLCbaojingxinxidizhi_clicked);
	connect(ui->btn_keyipaizhaoxinhaodizhi, &QPushButton::clicked, this, &DlgProductSet::btn_keyipaizhaoxinhaodizhi_clicked);
	connect(ui->btn_tingzhipaizhaoxinhaodizhi, &QPushButton::clicked, this, &DlgProductSet::btn_tingzhipaizhaoxinhaodizhi_clicked);
	connect(ui->btn_daizishicechangdudizhi, &QPushButton::clicked, this, &DlgProductSet::btn_daizishicechangdudizhi_clicked);
	connect(ui->btn_shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhi, &QPushButton::clicked, this, &DlgProductSet::btn_shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhi_clicked);

	// 写入PLC
	connect(ui->btn_shiceyahenkuanduxierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_shiceyahenkuanduxierushuzhi_clicked);
	connect(ui->btn_shedingyahenbiaozhunkuanduzhixierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_shedingyahenbiaozhunkuanduzhixierushuzhi_clicked);
	connect(ui->btn_paizhaojiangejulixierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_paizhaojiangejulixierushuzhi_clicked);
	connect(ui->btn_daizichangdubiaozhunxierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_daizichangdubiaozhunxierushuzhi_clicked);
	connect(ui->btn_jimiqiyiquanmaichongshuxierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_jimiqiyiquanmaichongshuxierushuzhi_clicked);
	connect(ui->btn_jimiqiyiquanzhouchangxierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_jimiqiyiquanzhouchangxierushuzhi_clicked);
	connect(ui->btn_bujinyiquanmaichongshuxierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_bujinyiquanmaichongshuxierushuzhi_clicked);
	connect(ui->btn_bujinluojuxierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_bujinluojuxierushuzhi_clicked);
	connect(ui->btn_daoyidongbuchangxierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_daoyidongbuchangxierushuzhi_clicked);
	connect(ui->btn_zidongdaoyidongzuidajulixierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_zidongdaoyidongzuidajulixierushuzhi_clicked);
	connect(ui->btn_daojiakeyidongdezuidajulixierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_daojiakeyidongdezuidajulixierushuzhi_clicked);
	connect(ui->btn_daoyidongsudumaichongpinlvxierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_daoyidongsudumaichongpinlvxierushuzhi_clicked);
	connect(ui->btn_shibiezhongxindianyutuxiangzhongxindianchazhixierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_shibiezhongxindianyutuxiangzhongxindianchazhixierushuzhi_clicked);

	connect(ui->btn_writeshiceyahenkuanduxierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_writeshiceyahenkuanduxierushuzhi_clicked);
	connect(ui->btn_writeshedingyahenbiaozhunkuanduzhixierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_writeshedingyahenbiaozhunkuanduzhixierushuzhi_clicked);
	connect(ui->btn_writepaizhaojiangejulixierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_writepaizhaojiangejulixierushuzhi_clicked);
	connect(ui->btn_writedaizichangdubiaozhunxierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_writedaizichangdubiaozhunxierushuzhi_clicked);
	connect(ui->btn_writejimiqiyiquanmaichongshuxierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_writejimiqiyiquanmaichongshuxierushuzhi_clicked);
	connect(ui->btn_writejimiqiyiquanzhouchangxierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_writejimiqiyiquanzhouchangxierushuzhi_clicked);
	connect(ui->btn_writebujinyiquanmaichongshuxierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_writebujinyiquanmaichongshuxierushuzhi_clicked);
	connect(ui->btn_writebujinluojuxierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_writebujinluojuxierushuzhi_clicked);
	connect(ui->btn_writedaoyidongbuchangxierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_writedaoyidongbuchangxierushuzhi_clicked);
	connect(ui->btn_writezidongdaoyidongzuidajulixierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_writezidongdaoyidongzuidajulixierushuzhi_clicked);
	connect(ui->btn_writedaojiakeyidongdezuidajulixierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_writedaojiakeyidongdezuidajulixierushuzhi_clicked);
	connect(ui->btn_writedaoyidongsudumaichongpinlvxierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_writedaoyidongsudumaichongpinlvxierushuzhi_clicked);
	connect(ui->btn_writeshibiezhongxindianyutuxiangzhongxindianchazhixierushuzhi, &QPushButton::clicked, this, &DlgProductSet::btn_writeshibiezhongxindianyutuxiangzhongxindianchazhixierushuzhi_clicked);


	connect(ui->ckb_autoSaveImg, &QCheckBox::clicked, this, &DlgProductSet::ckb_autoSaveImg_clicked);
	connect(ui->ckb_shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan, &QCheckBox::clicked, this, &DlgProductSet::ckb_shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan_clicked);
	connect(ui->tabWidget, &QTabWidget::currentChanged, this, &DlgProductSet::tabWidget_indexChanged);
}

void DlgProductSet::onUpdatePLCInfo(QVector<PlcReadItem> datas)
{
	auto applyValue = [](QPushButton* btn, const PlcReadItem& item)
		{
			if (item.ok)
			{
				btn->setText(QString::number(item.value / 100.0));
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
		case PlcReadIndex::shiceyahenkuandu:
			applyValue(ui->btn_shiceyahenkuanduduqushuzhi, item);
			break;
		case PlcReadIndex::shedingyahenbiaozhunzhi:
			applyValue(ui->btn_shedingyahenbiaozhunzhiduqushuzhi, item);
			break;
		case PlcReadIndex::paizhaojiangejuli:
			applyValue(ui->btn_paizhaojiangejuliduqushuzhi, item);
			break;
		case PlcReadIndex::daizichangdubiaozhun:
			applyValue(ui->btn_daizichangdubiaozhunduqushuzhi, item);
			break;
		case PlcReadIndex::jimiqiyiquanmaichongshu:
			applyValue(ui->btn_jimiqiyiquanmaichongshuduqushuzhi, item);
			break;
		case PlcReadIndex::jimiqiyiquanzhouchang:
			applyValue(ui->btn_jimiqiyiquanzhouchangduqushuzhi, item);
			break;
		case PlcReadIndex::bujinyiquanmaichongshu:
			applyValue(ui->btn_bujinyiquanmaichongshuduqushuzhi, item);
			break;
		case PlcReadIndex::bujinluoju:
			applyValue(ui->btn_bujinluojuduqushuzhi, item);
			break;
		case PlcReadIndex::daoyidongbuchang:
			applyValue(ui->btn_daoyidongbuchangduqushuzhi, item);
			break;
		case PlcReadIndex::zidongdaoyidongzuidajuli:
			applyValue(ui->btn_zidongdaoyidongzuidajuliduqushuzhi, item);
			break;
		case PlcReadIndex::daojiakeyidongdezuidajuli:
			applyValue(ui->btn_daojiakeyidongdezuidajuliduqushuzhi, item);
			break;
		case PlcReadIndex::daoyidongsudumaichongpinlv:
			applyValue(ui->btn_daoyidongsudumaichongpinlvduqushuzhi, item);
			break;
		case PlcReadIndex::readPLCbaojingxinxi:
			applyValue(ui->btn_huodePLCbaojingxinxishuzhi, item);
			break;
		case PlcReadIndex::readPLCkeyipaizhaoxinhao:
			applyValue(ui->btn_keyipaizhaoxinhaoshuzhi, item);
			break;
		case PlcReadIndex::readPLCtingzhipaizhaoxinhao:
			applyValue(ui->btn_tingzhipaizhaoxinhaoshuzhi, item);
			break;
		case PlcReadIndex::readPLCdaizishicechangdu:
			applyValue(ui->btn_daizishicechangdushuzhi, item);
			break;
		case PlcReadIndex::shibiezhongxindianyutuxiangzhongxindianchazhi:
			applyValue(ui->btn_shibiezhongxindianyutuxiangzhongxindianchazhiduqushuzhi, item);
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

void DlgProductSet::btn_shiceyahenkuanduduqudizhi_clicked()
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
		if (!checkIsPLCAddressSame(newAddress, "shiceyahenkuandu"))
		{
			return;
		}

		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_shiceyahenkuanduduqudizhi->setText(value);
		setConfig.shiceyahenkuanduduqudizhi = value.toInt();
		ModBusAddress::shiceyahenkuanduAddress = value.toInt();
	}
}

void DlgProductSet::btn_shedingyahenbiaozhunzhiduqudizhi_clicked()
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
		if (!checkIsPLCAddressSame(newAddress, "shedingyahenbiaozhunzhi"))
		{
			return;
		}

		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_shedingyahenbiaozhunzhiduqudizhi->setText(value);
		setConfig.shedingyahenbiaozhunzhiduqudizhi = value.toInt();
		ModBusAddress::shedingyahenbiaozhunkuanduzhiAddress = value.toInt();
	}
}

void DlgProductSet::btn_paizhaojiangejuliduqudizhi_clicked()
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
		if (!checkIsPLCAddressSame(newAddress, "paizhaojiangejuli"))
		{
			return;
		}

		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_paizhaojiangejuliduqudizhi->setText(value);
		setConfig.paizhaojiangejuliduqudizhi = value.toInt();
		ModBusAddress::paizhaojiangejuliAddress = value.toInt();
	}
}

void DlgProductSet::btn_daizichangdubiaozhunduqudizhi_clicked()
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
		if (!checkIsPLCAddressSame(newAddress, "daizichangdubiaozhun"))
		{
			return;
		}

		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_daizichangdubiaozhunduqudizhi->setText(value);
		setConfig.daizichangdubiaozhunduqudizhi = value.toInt();
		ModBusAddress::daizichangdubiaozhunAddress = value.toInt();
	}
}

void DlgProductSet::btn_jimiqiyiquanmaichongshuduqudizhi_clicked()
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
		if (!checkIsPLCAddressSame(newAddress, "jimiqiyiquanmaichongshu"))
		{
			return;
		}

		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_jimiqiyiquanmaichongshuduqudizhi->setText(value);
		setConfig.jimiqiyiquanmaichongshuduqudizhi = value.toInt();
		ModBusAddress::jimiqiyiquanmaichongshuAddress = value.toInt();
	}
}

void DlgProductSet::btn_jimiqiyiquanzhouchangduqudizhi_clicked()
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
		if (!checkIsPLCAddressSame(newAddress, "jimiqiyiquanzhouchang"))
		{
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_jimiqiyiquanzhouchangduqudizhi->setText(value);
		setConfig.jimiqiyiquanzhouchangduqudizhi = value.toInt();
		ModBusAddress::jimiqiyiquanzhouchangAddress = value.toInt();
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
		if (!checkIsPLCAddressSame(newAddress, "bujinyiquanmaichongshu"))
		{
			return;
		}

		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_bujinyiquanmaichongshuduqudizhi->setText(value);
		setConfig.bujinyiquanmaichongshuduqudizhi = value.toInt();
		ModBusAddress::bujinyiquanmaichongshuAddress = value.toInt();
	}
}

void DlgProductSet::btn_bujinluojuduqudizhi_clicked()
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
		if (!checkIsPLCAddressSame(newAddress, "bujinluoju"))
		{
			return;
		}

		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_bujinluojuduqudizhi->setText(value);
		setConfig.bujinluojuduqudizhi = value.toInt();
		ModBusAddress::bujinluojuAddress = value.toInt();
	}
}

void DlgProductSet::btn_daoyidongbuchangduqudizhi_clicked()
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
		if (!checkIsPLCAddressSame(newAddress, "daoyidongbuchang"))
		{
			return;
		}

		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_daoyidongbuchangduqudizhi->setText(value);
		setConfig.daoyidongbuchangduqudizhi = value.toInt();
		ModBusAddress::daoyidongbuchangAddress = value.toInt();
	}
}

void DlgProductSet::btn_zidongdaoyidongzuidajuliduqudizhi_clicked()
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
		if (!checkIsPLCAddressSame(newAddress, "zidongdaoyidongzuidajuli"))
		{
			return;
		}

		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_zidongdaoyidongzuidajuliduqudizhi->setText(value);
		setConfig.zidongdaoyidongzuidajuliduqudizhi = value.toInt();
		ModBusAddress::zidongdaoyidongzuidajuliAddress = value.toInt();
	}
}

void DlgProductSet::btn_daojiakeyidongdezuidajuliduqudizhi_clicked()
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
		if (!checkIsPLCAddressSame(newAddress, "daojiakeyidongdezuidajuli"))
		{
			return;
		}

		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_daojiakeyidongdezuidajuliduqudizhi->setText(value);
		setConfig.daojiakeyidongdezuidajuliduqudizhi = value.toInt();
		ModBusAddress::daojiakeyidongdezuidajuliAddress = value.toInt();
	}
}

void DlgProductSet::btn_daoyidongsudumaichongpinlvduqudizhi_clicked()
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
		if (!checkIsPLCAddressSame(newAddress, "daoyidongsudumaichongpinlv"))
		{
			return;
		}

		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_daoyidongsudumaichongpinlvduqudizhi->setText(value);
		setConfig.daoyidongsudumaichongpinlvduqudizhi = value.toInt();
		ModBusAddress::daoyidongsudumaichongpinlvAddress = value.toInt();
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
		if (!checkIsPLCAddressSame(newAddress, "huodePLCbaojingxinxi"))
		{
			return;
		}

		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_huodePLCbaojingxinxidizhi->setText(value);
		setConfig.huodePLCbaojingxinxidizhi = value.toInt();
		ModBusAddress::readPLCbaojingxinxiAddress = value.toInt();
	}
}

void DlgProductSet::btn_keyipaizhaoxinhaodizhi_clicked()
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
		if (!checkIsPLCAddressSame(newAddress, "keyipaizhaoxinhao"))
		{
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_keyipaizhaoxinhaodizhi->setText(value);
		setConfig.keyipaizhaoxinhaodizhi = value.toInt();
		ModBusAddress::readPLCkeyipaizhaoxinhaoAddress = value.toInt();
	}
}

void DlgProductSet::btn_tingzhipaizhaoxinhaodizhi_clicked()
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
		if (!checkIsPLCAddressSame(newAddress, "tingzhipaizhaoxinhao"))
		{
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_tingzhipaizhaoxinhaodizhi->setText(value);
		setConfig.tingzhipaizhaoxinhaodizhi = value.toInt();
		ModBusAddress::readPLCtingzhipaizhaoxinhaoAddress = value.toInt();
	}
}

void DlgProductSet::btn_daizishicechangdudizhi_clicked()
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
		if (!checkIsPLCAddressSame(newAddress, "daizishicechangdu"))
		{
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_daizishicechangdudizhi->setText(value);
		setConfig.daizishicechangdudizhi = value.toInt();
		ModBusAddress::readPLCdaizishicechangduAddress = value.toInt();
	}
}

void DlgProductSet::btn_shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhi_clicked()
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
		if (!checkIsPLCAddressSame(newAddress, "shibiezhongxindianyutuxiangzhongxindianchazhi"))
		{
			return;
		}

		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhi->setText(value);
		setConfig.shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhi = value.toInt();
		ModBusAddress::shibiezhongxindianyutuxiangzhongxindianchazhiAddress = value.toInt();
	}
}

void DlgProductSet::btn_shiceyahenkuanduxierushuzhi_clicked()
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
		ui->btn_shiceyahenkuanduxierushuzhi->setText(value);
	}
}

void DlgProductSet::btn_shedingyahenbiaozhunkuanduzhixierushuzhi_clicked()
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
		ui->btn_shedingyahenbiaozhunkuanduzhixierushuzhi->setText(value);
	}
}

void DlgProductSet::btn_paizhaojiangejulixierushuzhi_clicked()
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
		ui->btn_paizhaojiangejulixierushuzhi->setText(value);
	}
}

void DlgProductSet::btn_daizichangdubiaozhunxierushuzhi_clicked()
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
		ui->btn_daizichangdubiaozhunxierushuzhi->setText(value);
	}
}

void DlgProductSet::btn_jimiqiyiquanmaichongshuxierushuzhi_clicked()
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
		ui->btn_jimiqiyiquanmaichongshuxierushuzhi->setText(value);
	}
}

void DlgProductSet::btn_jimiqiyiquanzhouchangxierushuzhi_clicked()
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
		ui->btn_jimiqiyiquanzhouchangxierushuzhi->setText(value);
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

void DlgProductSet::btn_bujinluojuxierushuzhi_clicked()
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
		ui->btn_bujinluojuxierushuzhi->setText(value);
	}
}

void DlgProductSet::btn_daoyidongbuchangxierushuzhi_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		/*if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于等于0的数值");
			return;
		}*/
		ui->btn_daoyidongbuchangxierushuzhi->setText(value);
	}
}

void DlgProductSet::btn_zidongdaoyidongzuidajulixierushuzhi_clicked()
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
		ui->btn_zidongdaoyidongzuidajulixierushuzhi->setText(value);
	}
}

void DlgProductSet::btn_daojiakeyidongdezuidajulixierushuzhi_clicked()
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
		ui->btn_daojiakeyidongdezuidajulixierushuzhi->setText(value);
	}
}

void DlgProductSet::btn_daoyidongsudumaichongpinlvxierushuzhi_clicked()
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
		ui->btn_daoyidongsudumaichongpinlvxierushuzhi->setText(value);
	}
}

void DlgProductSet::btn_shibiezhongxindianyutuxiangzhongxindianchazhixierushuzhi_clicked()
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
		ui->btn_shibiezhongxindianyutuxiangzhongxindianchazhixierushuzhi->setText(value);
	}
}

void DlgProductSet::btn_writeshiceyahenkuanduxierushuzhi_clicked()
{
	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;

	if (plcControllerScheduler)
	{
		uint16_t writeData = ui->btn_shiceyahenkuanduxierushuzhi->text().toUInt();
		auto fut = plcControllerScheduler->writeUInt16RegisterAsync(ModBusAddress::shiceyahenkuanduAddress, writeData * 100);

		if (fut.get())
		{
			QMessageBox::information(this, "提示", "实测压痕宽度写入成功");
		}
		else
		{
			QMessageBox::information(this, "警告", "实测压痕宽度写入失败");
		}
	}
	else
	{
		QMessageBox::information(this, "警告", "PLC未连接");
	}
}

void DlgProductSet::btn_writeshedingyahenbiaozhunkuanduzhixierushuzhi_clicked()
{
	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;

	if (plcControllerScheduler)
	{
		uint16_t writeData = ui->btn_shedingyahenbiaozhunkuanduzhixierushuzhi->text().toUInt();
		auto fut = plcControllerScheduler->writeUInt16RegisterAsync(ModBusAddress::shedingyahenbiaozhunkuanduzhiAddress, writeData * 100);

		if (fut.get())
		{
			QMessageBox::information(this, "提示", "设定压痕标准宽度值写入成功");
		}
		else
		{
			QMessageBox::information(this, "警告", "设定压痕标准宽度值写入失败");
		}
	}
	else
	{
		QMessageBox::information(this, "警告", "PLC未连接");
	}
}

void DlgProductSet::btn_writepaizhaojiangejulixierushuzhi_clicked()
{
	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;

	if (plcControllerScheduler)
	{
		uint16_t writeData = ui->btn_paizhaojiangejulixierushuzhi->text().toUInt();
		auto fut = plcControllerScheduler->writeUInt16RegisterAsync(ModBusAddress::paizhaojiangejuliAddress, writeData * 100);

		if (fut.get())
		{
			QMessageBox::information(this, "提示", "拍照间隔距离写入成功");
		}
		else
		{
			QMessageBox::information(this, "警告", "拍照间隔距离写入失败");
		}
	}
	else
	{
		QMessageBox::information(this, "警告", "PLC未连接");
	}
}

void DlgProductSet::btn_writedaizichangdubiaozhunxierushuzhi_clicked()
{
	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;

	if (plcControllerScheduler)
	{
		uint16_t writeData = ui->btn_daizichangdubiaozhunxierushuzhi->text().toUInt();
		auto fut = plcControllerScheduler->writeUInt16RegisterAsync(ModBusAddress::daizichangdubiaozhunAddress, writeData * 100);

		if (fut.get())
		{
			QMessageBox::information(this, "提示", "袋子长度标准写入成功");
		}
		else
		{
			QMessageBox::information(this, "警告", "袋子长度标准写入失败");
		}
	}
	else
	{
		QMessageBox::information(this, "警告", "PLC未连接");
	}
}

void DlgProductSet::btn_writejimiqiyiquanmaichongshuxierushuzhi_clicked()
{
	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;

	if (plcControllerScheduler)
	{
		uint16_t writeData = ui->btn_jimiqiyiquanmaichongshuxierushuzhi->text().toUInt();
		auto fut = plcControllerScheduler->writeUInt16RegisterAsync(ModBusAddress::jimiqiyiquanmaichongshuAddress, writeData * 100);

		if (fut.get())
		{
			QMessageBox::information(this, "提示", "计米器一圈脉冲数写入成功");
		}
		else
		{
			QMessageBox::information(this, "警告", "计米器一圈脉冲数写入失败");
		}
	}
	else
	{
		QMessageBox::information(this, "警告", "PLC未连接");
	}
}

void DlgProductSet::btn_writejimiqiyiquanzhouchangxierushuzhi_clicked()
{
	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;

	if (plcControllerScheduler)
	{
		uint16_t writeData = ui->btn_jimiqiyiquanzhouchangxierushuzhi->text().toUInt();
		auto fut = plcControllerScheduler->writeUInt16RegisterAsync(ModBusAddress::jimiqiyiquanzhouchangAddress, writeData * 100);

		if (fut.get())
		{
			QMessageBox::information(this, "提示", "计米器一圈周长写入成功");
		}
		else
		{
			QMessageBox::information(this, "警告", "计米器一圈周长写入失败");
		}
	}
	else
	{
		QMessageBox::information(this, "警告", "PLC未连接");
	}
}

void DlgProductSet::btn_writebujinyiquanmaichongshuxierushuzhi_clicked()
{
	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;
	if (plcControllerScheduler)
	{
		uint16_t writeData = ui->btn_bujinyiquanmaichongshuxierushuzhi->text().toUInt();
		auto fut = plcControllerScheduler->writeUInt16RegisterAsync(ModBusAddress::bujinyiquanmaichongshuAddress, writeData * 100);
		if (fut.get())
		{
			QMessageBox::information(this, "提示", "步进一圈脉冲数写入成功");
		}
		else
		{
			QMessageBox::information(this, "警告", "步进一圈脉冲数写入失败");
		}
	}
	else
	{
		QMessageBox::information(this, "警告", "PLC未连接");
	}
}

void DlgProductSet::btn_writebujinluojuxierushuzhi_clicked()
{
	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;
	if (plcControllerScheduler)
	{
		uint16_t writeData = ui->btn_bujinluojuxierushuzhi->text().toUInt();
		auto fut = plcControllerScheduler->writeUInt16RegisterAsync(ModBusAddress::bujinluojuAddress, writeData * 100);
		if (fut.get())
		{
			QMessageBox::information(this, "提示", "步进螺距写入成功");
		}
		else
		{
			QMessageBox::information(this, "警告", "步进螺距写入失败");
		}
	}
	else
	{
		QMessageBox::information(this, "警告", "PLC未连接");
	}
}

void DlgProductSet::btn_writedaoyidongbuchangxierushuzhi_clicked()
{
	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;
	if (plcControllerScheduler)
	{
		uint16_t writeData = ui->btn_daoyidongbuchangxierushuzhi->text().toUInt();
		auto fut = plcControllerScheduler->writeUInt16RegisterAsync(ModBusAddress::daoyidongbuchangAddress, writeData * 100);
		if (fut.get())
		{
			QMessageBox::information(this, "提示", "刀移动补偿写入成功");
		}
		else
		{
			QMessageBox::information(this, "警告", "刀移动补偿写入失败");
		}
	}
	else
	{
		QMessageBox::information(this, "警告", "PLC未连接");
	}
}

void DlgProductSet::btn_writezidongdaoyidongzuidajulixierushuzhi_clicked()
{
	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;
	if (plcControllerScheduler)
	{
		uint16_t writeData = ui->btn_zidongdaoyidongzuidajulixierushuzhi->text().toUInt();
		auto fut = plcControllerScheduler->writeUInt16RegisterAsync(ModBusAddress::zidongdaoyidongzuidajuliAddress, writeData * 100);
		if (fut.get())
		{
			QMessageBox::information(this, "提示", "自动刀移动最大距离写入成功");
		}
		else
		{
			QMessageBox::information(this, "警告", "自动刀移动最大距离写入失败");
		}
	}
	else
	{
		QMessageBox::information(this, "警告", "PLC未连接");
	}
}

void DlgProductSet::btn_writedaojiakeyidongdezuidajulixierushuzhi_clicked()
{
	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;
	if (plcControllerScheduler)
	{
		uint16_t writeData = ui->btn_daojiakeyidongdezuidajulixierushuzhi->text().toUInt();
		auto fut = plcControllerScheduler->writeUInt16RegisterAsync(ModBusAddress::daojiakeyidongdezuidajuliAddress, writeData * 100);
		if (fut.get())
		{
			QMessageBox::information(this, "提示", "刀架可移动的最大距离写入成功");
		}
		else
		{
			QMessageBox::information(this, "警告", "刀架可移动的最大距离写入失败");
		}
	}
	else
	{
		QMessageBox::information(this, "警告", "PLC未连接");
	}
}

void DlgProductSet::btn_writedaoyidongsudumaichongpinlvxierushuzhi_clicked()
{
	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;
	if (plcControllerScheduler)
	{
		uint16_t writeData = ui->btn_daoyidongsudumaichongpinlvxierushuzhi->text().toUInt();
		auto fut = plcControllerScheduler->writeUInt16RegisterAsync(ModBusAddress::daoyidongsudumaichongpinlvAddress, writeData * 100);
		if (fut.get())
		{
			QMessageBox::information(this, "提示", "刀移动速度脉冲频率写入成功");
		}
		else
		{
			QMessageBox::information(this, "警告", "刀移动速度脉冲频率写入失败");
		}
	}
	else
	{
		QMessageBox::information(this, "警告", "PLC未连接");
	}
}

void DlgProductSet::btn_writeshibiezhongxindianyutuxiangzhongxindianchazhixierushuzhi_clicked()
{
	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;

	if (plcControllerScheduler)
	{
		uint16_t writeData = ui->btn_shibiezhongxindianyutuxiangzhongxindianchazhixierushuzhi->text().toUInt();
		auto fut = plcControllerScheduler->writeUInt16RegisterAsync(ModBusAddress::shibiezhongxindianyutuxiangzhongxindianchazhiAddress, writeData * 100);

		if (fut.get())
		{
			QMessageBox::information(this, "提示", "识别中心点与图像中心点差值写入成功");
		}
		else
		{
			QMessageBox::information(this, "警告", "识别中心点与图像中心点差值写入失败");
		}
	}
	else
	{
		QMessageBox::information(this, "警告", "PLC未连接");
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
		{ "shiceyahenkuandu",				 "实测压痕宽度读取地址",			   setConfig.shiceyahenkuanduduqudizhi },
		{ "shedingyahenbiaozhunzhi",		 "设定压痕标准值读取地址",		   setConfig.shedingyahenbiaozhunzhiduqudizhi },
		{ "paizhaojiangejuli",			 "拍照间隔距离读取地址",			   setConfig.paizhaojiangejuliduqudizhi },
		{ "daizichangdubiaozhun",			 "袋子长度标准读取地址",			setConfig.daizichangdubiaozhunduqudizhi },
		{ "jimiqiyiquanmaichongshu",		 "计米器一圈脉冲数读取地址",		  setConfig.jimiqiyiquanmaichongshuduqudizhi },
		{ "jimiqiyiquanzhouchang",		 "计米器一圈周长读取地址",             setConfig.jimiqiyiquanzhouchangduqudizhi },
		{ "bujinyiquanmaichongshu",		 "步进一圈脉冲数读取地址",      setConfig.bujinyiquanmaichongshuduqudizhi },
		
		{ "bujinluoju",					 "步进螺距读取地址",      setConfig.bujinluojuduqudizhi },
		{ "daoyidongbuchang",				 "刀移动补偿读取地址",      setConfig.daoyidongbuchangduqudizhi },
		{ "zidongdaoyidongzuidajuli",		 "自动刀移动最大距离读取地址",      setConfig.zidongdaoyidongzuidajuliduqudizhi },
		{ "daojiakeyidongdezuidajuli",	 "刀架可移动的最大距离读取地址",      setConfig.daojiakeyidongdezuidajuliduqudizhi },
		{ "daoyidongsudumaichongpinlv",	 "刀移动速度脉冲频率读取地址",      setConfig.daoyidongsudumaichongpinlvduqudizhi },
		{ "shibiezhongxindianyutuxiangzhongxindianchazhi", "识别中心点与图像中心点差值读取地址", setConfig.shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhi },

		{ "huodePLCbaojingxinxi",			 "获得PLC报警信息地址",		 setConfig.huodePLCbaojingxinxidizhi },
		{ "keyipaizhaoxinhao",			 "可以拍照信号地址",			 setConfig.keyipaizhaoxinhaodizhi },
		{ "tingzhipaizhaoxinhao",			 "停止拍照信号地址",			 setConfig.tingzhipaizhaoxinhaodizhi },
		{ "daizishicechangdu",			 "袋子实测长度地址",			 setConfig.daizishicechangdudizhi }

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

void DlgProductSet::ckb_shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan_clicked()
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	setConfig.shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan = ui->ckb_shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan->isChecked();
}
