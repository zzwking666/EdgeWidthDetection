#include "DlgProductSet.h"
#include "ui_DlgProductSet.h"

#include <QFutureWatcher>
#include <QGroupBox>
#include <QMessageBox>
#include <QTabWidget>
#include <QtConcurrent/qtconcurrentrun.h>
#include <future>
#include "Modules.hpp"
#include "NumberKeyboard.h"
#include "Utilty.hpp"

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
	buildCircularWriteUi();
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
	ui->ckb_yixiangjishibiezhongxindianyutuxiangzhongxindianchazhishifouqufan->setChecked(setConfig.shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan1);

	// 相机参数
	ui->btn_shangxianwei1->setText(QString::number(setConfig.shangxianwei1));
	ui->btn_xiaxianwei1->setText(QString::number(setConfig.xiaxianwei1));
	ui->btn_zuoxianwei1->setText(QString::number(setConfig.zuoxianwei1));
	ui->btn_youxianwei1->setText(QString::number(setConfig.youxianwei1));
	ui->btn_xiangsudangliang1->setText(QString::number(setConfig.xiangsudangliang1));
	ui->btn_debugFrame1->setText(QString::number(setConfig.debugFrame1));
	ui->btn_openRemoveFrame1->setText(QString::number(setConfig.openRemoveFrame1));

	// 亮度调整 
	ui->btn_zengyi1->setText(QString::number(setConfig.zengyi1));
	ui->btn_qiangguang1->setText(QString::number(setConfig.qiangguang1));
	ui->btn_zhongguang1->setText(QString::number(setConfig.zhongguang1));
	ui->btn_ruoguang1->setText(QString::number(setConfig.ruoguang1));

	// 相机2参数
	ui->btn_shangxianwei2->setText(QString::number(setConfig.shangxianwei2));
	ui->btn_xiaxianwei2->setText(QString::number(setConfig.xiaxianwei2));
	ui->btn_zuoxianwei2->setText(QString::number(setConfig.zuoxianwei2));
	ui->btn_youxianwei2->setText(QString::number(setConfig.youxianwei2));
	ui->btn_xiangsudangliang2->setText(QString::number(setConfig.xiangsudangliang2));
	ui->btn_debugFrame2->setText(QString::number(setConfig.debugFrame2));
	ui->btn_openRemoveFrame2->setText(QString::number(setConfig.openRemoveFrame2));

	// 相机2亮度调整
	ui->btn_zengyi2->setText(QString::number(setConfig.zengyi2));
	ui->btn_qiangguang2->setText(QString::number(setConfig.qiangguang2));
	ui->btn_zhongguang2->setText(QString::number(setConfig.zhongguang2));
	ui->btn_ruoguang2->setText(QString::number(setConfig.ruoguang2));

	// 相机2识别中心点与图像中心点差值是否取反
	ui->ckb_shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan_2->setChecked(setConfig.shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan2);


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

void DlgProductSet::save_config()
{
	// 使用多代备份 + 写入后验证的安全保存，防止断电导致配置文件损坏
	Modules::getInstance().configManagerModule.saveConfigSafe();
}

void DlgProductSet::buildCircularWriteUi()
{
	// 三个功能固定各占 60 个 Modbus 地址（仅写偶数地址）。
	// 显示区分组框与 60 个按钮已在 DlgProductSet.ui 中静态布局，这里按 objectName 查找并缓存，
	// 下标 i 对应功能块内第 i 个地址（基地址 = funcIndex * 60）
	struct FuncUiInfo
	{
		QGroupBox* groupBox;	// 分组框（按钮的查找父对象）
		QString prefix;			// 按钮 objectName 前缀
	};
	const FuncUiInfo funcs[] = {
		{ ui->groupBox_lengdaoyahen,      QStringLiteral("btn_lengdaoyahen_") },		// 0=冷刀压痕 0~59
		{ ui->groupBox_zhongxinpianyizhi, QStringLiteral("btn_zhongxinpianyizhi_") },	// 1=中心偏移值 60~119
		{ ui->groupBox_qiedaoyahen,       QStringLiteral("btn_qiedaoyahen_") },		// 2=切刀压痕 120~179
	};

	for (int f = 0; f < 3; ++f)
	{
		_plcCircularButtons[f].resize(60);
		for (int i = 0; i < 60; ++i)
		{
			_plcCircularButtons[f][i] = funcs[f].groupBox->findChild<QPushButton*>(funcs[f].prefix + QString::number(i));
		}
	}
}

void DlgProductSet::onPlcCircularWrite(int funcIndex, int writeAddress, double value, int clearAddress)
{
	if (funcIndex < 0 || funcIndex >= 3)
	{
		return;
	}
	const int baseAddress = funcIndex * 60;
	auto& buttons = _plcCircularButtons[funcIndex];

	// 先清零旧槽位再显示新写入值（与 PLC 侧先清后写顺序一致，两槽相同时保证显示的是写入值）
	const int clearIndex = clearAddress - baseAddress;
	if (clearIndex >= 0 && clearIndex < buttons.size() && buttons[clearIndex])
	{
		buttons[clearIndex]->setText("");
	}
	const int writeIndex = writeAddress - baseAddress;
	if (writeIndex >= 0 && writeIndex < buttons.size() && buttons[writeIndex])
	{
		buttons[writeIndex]->setText(QString::number(value, 'f', 2));
	}
}

void DlgProductSet::onPlcRealtimeWrite(int address, double value)
{
	QPushButton* button = nullptr;
	bool isFlag = false;	// 206/208 为完成状态标志，按整数显示
	switch (address)
	{
	case 200: button = ui->btn_shishixieru_lengdaoyahen; break;			// 1相机压痕（冷刀）
	case 202: button = ui->btn_shishixieru_zhongxinpianyizhi; break;	// 1相机中心偏移值
	case 204: button = ui->btn_shishixieru_qiedaoyahen; break;			// 2相机压痕（切刀）
	case 206: button = ui->btn_shishixieru_wanchengbiaozhi; isFlag = true; break;	// 1相机写入完成标志
	case 208: button = ui->btn_shishixieru_wanchengbiaozhi2; isFlag = true; break;	// 2相机写入状态标志
	default: return;
	}
	if (button)
	{
		button->setText(isFlag ? QString::number(static_cast<int>(value))
			: QString::number(value, 'f', 2));
	}
}

void DlgProductSet::closeEvent(QCloseEvent* event)
{
	// 关闭窗口前保存 SetConfig 参数到本地
	save_config();
	QDialog::closeEvent(event);
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
	connect(ui->btn_zengyi1, &QPushButton::clicked, this, &DlgProductSet::btn_zengyi1_clicked);
	connect(ui->btn_xiangsudangliang1, &QPushButton::clicked, this, &DlgProductSet::btn_xiangsudangliang1_clicked);
	connect(ui->btn_debugFrame1, &QPushButton::clicked, this, &DlgProductSet::btn_debugFrame1_clicked);
	connect(ui->btn_openRemoveFrame1, &QPushButton::clicked, this, &DlgProductSet::btn_openRemoveFrame1_clicked);
	connect(ui->btn_qiangguang1, &QPushButton::clicked, this, &DlgProductSet::btn_qiangguang1_clicked);
	connect(ui->btn_zhongguang1, &QPushButton::clicked, this, &DlgProductSet::btn_zhongguang1_clicked);
	connect(ui->btn_ruoguang1, &QPushButton::clicked, this, &DlgProductSet::btn_ruoguang1_clicked);

	// 相机2
	connect(ui->btn_testTrigger1_2, &QPushButton::clicked, this, &DlgProductSet::btn_testTrigger1_2_clicked);
	connect(ui->btn_testTrigger2_2, &QPushButton::clicked, this, &DlgProductSet::btn_testTrigger2_2_clicked);
	connect(ui->btn_shangxianwei2, &QPushButton::clicked, this, &DlgProductSet::btn_shangxianwei2_clicked);
	connect(ui->btn_xiaxianwei2, &QPushButton::clicked, this, &DlgProductSet::btn_xiaxianwei2_clicked);
	connect(ui->btn_zuoxianwei2, &QPushButton::clicked, this, &DlgProductSet::btn_zuoxianwei2_clicked);
	connect(ui->btn_youxianwei2, &QPushButton::clicked, this, &DlgProductSet::btn_youxianwei2_clicked);
	connect(ui->btn_zengyi2, &QPushButton::clicked, this, &DlgProductSet::btn_zengyi2_clicked);
	connect(ui->btn_xiangsudangliang2, &QPushButton::clicked, this, &DlgProductSet::btn_xiangsudangliang2_clicked);
	connect(ui->btn_debugFrame2, &QPushButton::clicked, this, &DlgProductSet::btn_debugFrame2_clicked);
	connect(ui->btn_openRemoveFrame2, &QPushButton::clicked, this, &DlgProductSet::btn_openRemoveFrame2_clicked);
	connect(ui->btn_qiangguang2, &QPushButton::clicked, this, &DlgProductSet::btn_qiangguang2_clicked);
	connect(ui->btn_zhongguang2, &QPushButton::clicked, this, &DlgProductSet::btn_zhongguang2_clicked);
	connect(ui->btn_ruoguang2, &QPushButton::clicked, this, &DlgProductSet::btn_ruoguang2_clicked);
	connect(ui->ckb_shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan_2, &QCheckBox::clicked, this, &DlgProductSet::ckb_shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan2_clicked);


	connect(ui->ckb_autoSaveImg, &QCheckBox::clicked, this, &DlgProductSet::ckb_autoSaveImg_clicked);
	connect(ui->btn_testPlcAddressWrite, &QPushButton::clicked, this, &DlgProductSet::btn_testPlcAddressWrite_clicked);
	connect(ui->ckb_yixiangjishibiezhongxindianyutuxiangzhongxindianchazhishifouqufan, &QCheckBox::clicked, this, &DlgProductSet::ckb_shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan_clicked);
	connect(ui->tabWidget, &QTabWidget::currentChanged, this, &DlgProductSet::tabWidget_indexChanged);
}

void DlgProductSet::btn_close_clicked()
{
	emit paramsChanged();

	// 实机未使用 PLC 轮询监听线程，先整体注释掉（不再创建该线程）
	//auto& plcListenThread = Modules::getInstance().plcController.plcListenThread;
	//if (plcListenThread)
	//{
	//	plcListenThread->stopThread();
	//}

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

void DlgProductSet::btn_qiangguang1_clicked()
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
		ui->btn_qiangguang1->setText(value);
		setConfig.qiangguang1 = value.toDouble();
	}
}

void DlgProductSet::btn_zhongguang1_clicked()
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
		ui->btn_zhongguang1->setText(value);
		setConfig.zhongguang1 = value.toDouble();
	}
}

void DlgProductSet::btn_ruoguang1_clicked()
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
		ui->btn_ruoguang1->setText(value);
		setConfig.ruoguang1 = value.toDouble();
	}
}


void DlgProductSet::tabWidget_indexChanged(int index)
{
	// 实机未使用 PLC 轮询监听线程，先整体注释掉（不再创建该线程）
	//if (2 == index)
	//{
	//	auto& plcListenThread = Modules::getInstance().plcController.plcListenThread;
	//	if (plcListenThread)
	//	{
	//		plcListenThread->startThread();
	//	}
	//}
	//else
	//{
	//	auto& plcListenThread = Modules::getInstance().plcController.plcListenThread;
	//	if (plcListenThread)
	//	{
	//		plcListenThread->stopThread();
	//	}
	//}
}

void DlgProductSet::btn_testPlcAddressWrite_clicked()
{
	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;
	if (!plcControllerScheduler)
	{
		QMessageBox::information(this, "警告", "PLC未连接");
		return;
	}

	// 测试写入：向 0~179 每个地址写入其地址值（0 地址写 0，179 地址写 179），
	// 覆盖一相机（冷刀压痕 0~59、中心偏移值 60~119）与二相机（切刀压痕 120~179）的全部地址
	std::vector<std::future<bool>> futs;
	futs.reserve(180);
	for (int addr = 0; addr < 180; ++addr)
	{
		futs.push_back(plcControllerScheduler->writeUInt16RegisterAsync(
			static_cast<uint16_t>(addr), static_cast<uint16_t>(addr)));

		// 写入值回显到对应地址的按钮上
		auto* btn = _plcCircularButtons[addr / 60][addr % 60];
		if (btn)
		{
			btn->setText(QString::number(addr));
		}
	}

	// 后台等待全部写入完成并统计失败数，避免阻塞 UI 线程
	auto* watcher = new QFutureWatcher<int>(this);
	connect(watcher, &QFutureWatcher<int>::finished, this, [this, watcher]()
		{
			const int failCount = watcher->result();
			watcher->deleteLater();
			if (failCount == 0)
			{
				QMessageBox::information(this, "提示", "测试写入完成：0~179 全部写入成功");
			}
			else
			{
				QMessageBox::warning(this, "警告",
					QString("测试写入完成：%1 个地址写入失败").arg(failCount));
			}
		});
	watcher->setFuture(QtConcurrent::run([futs = std::move(futs)]() mutable
		{
			int failCount = 0;
			for (auto& fut : futs)
			{
				if (!fut.get())
				{
					++failCount;
				}
			}
			return failCount;
		}));
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

void DlgProductSet::btn_debugFrame1_clicked()
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
		if (value.toDouble() > 3)
		{
			QMessageBox::warning(this, "提示", "调试模式帧数最高只能设置为3");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_debugFrame1->setText(value);
		setConfig.debugFrame1 = value.toDouble();
		auto& runningState = Modules::getInstance().runtimeInfoModule.runningState;
		if (runningState == RunningState::Debug)
		{
			auto& camera = Modules::getInstance().cameraModule.camera1;
			if (camera)
			{
				camera->setFrameRate(setConfig.debugFrame1);
			}
		}
	}
}

void DlgProductSet::btn_openRemoveFrame1_clicked()
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
		ui->btn_openRemoveFrame1->setText(value);
		setConfig.openRemoveFrame1 = value.toDouble();
		auto& runningState = Modules::getInstance().runtimeInfoModule.runningState;
		if (runningState == RunningState::OpenRemoveFunc)
		{
			auto& camera = Modules::getInstance().cameraModule.camera1;
			if (camera)
			{
				camera->setFrameRate(setConfig.openRemoveFrame1);
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
	setConfig.shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan1 = ui->ckb_yixiangjishibiezhongxindianyutuxiangzhongxindianchazhishifouqufan->isChecked();
}

void DlgProductSet::btn_testTrigger1_2_clicked()
{
	auto& camera = Modules::getInstance().cameraModule.camera2;
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

void DlgProductSet::btn_testTrigger2_2_clicked()
{
	auto& camera = Modules::getInstance().cameraModule.camera2;
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

void DlgProductSet::btn_shangxianwei2_clicked()
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
		ui->btn_shangxianwei2->setText(value);
		setConfig.shangxianwei2 = value.toDouble();
	}
}

void DlgProductSet::btn_xiaxianwei2_clicked()
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
		ui->btn_xiaxianwei2->setText(value);
		setConfig.xiaxianwei2 = value.toDouble();
	}
}

void DlgProductSet::btn_zuoxianwei2_clicked()
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
		ui->btn_zuoxianwei2->setText(value);
		setConfig.zuoxianwei2 = value.toDouble();
	}
}

void DlgProductSet::btn_youxianwei2_clicked()
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
		ui->btn_youxianwei2->setText(value);
		setConfig.youxianwei2 = value.toDouble();
	}
}

void DlgProductSet::btn_zengyi2_clicked()
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
		auto& camera = Modules::getInstance().cameraModule.camera2;
		if (camera)
		{
			camera->setGain(value.toInt());
		}
		ui->btn_zengyi2->setText(value);
		setConfig.zengyi2 = value.toDouble();
	}
}

void DlgProductSet::btn_qiangguang2_clicked()
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
		ui->btn_qiangguang2->setText(value);
		setConfig.qiangguang2 = value.toDouble();
	}
}

void DlgProductSet::btn_zhongguang2_clicked()
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
		ui->btn_zhongguang2->setText(value);
		setConfig.zhongguang2 = value.toDouble();
	}
}

void DlgProductSet::btn_ruoguang2_clicked()
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
		ui->btn_ruoguang2->setText(value);
		setConfig.ruoguang2 = value.toDouble();
	}
}

void DlgProductSet::btn_xiangsudangliang2_clicked()
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
		ui->btn_xiangsudangliang2->setText(value);
		setConfig.xiangsudangliang2 = value.toDouble();
	}
}

void DlgProductSet::btn_debugFrame2_clicked()
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
		if (value.toDouble() > 3)
		{
			QMessageBox::warning(this, "提示", "调试模式帧数最高只能设置为3");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_debugFrame2->setText(value);
		setConfig.debugFrame2 = value.toDouble();
		auto& runningState = Modules::getInstance().runtimeInfoModule.runningState;
		if (runningState == RunningState::Debug)
		{
			auto& camera = Modules::getInstance().cameraModule.camera2;
			if (camera)
			{
				camera->setFrameRate(setConfig.debugFrame2);
			}
		}
	}
}

void DlgProductSet::btn_openRemoveFrame2_clicked()
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
		ui->btn_openRemoveFrame2->setText(value);
		setConfig.openRemoveFrame2 = value.toDouble();
		auto& runningState = Modules::getInstance().runtimeInfoModule.runningState;
		if (runningState == RunningState::OpenRemoveFunc)
		{
			auto& camera = Modules::getInstance().cameraModule.camera2;
			if (camera)
			{
				camera->setFrameRate(setConfig.openRemoveFrame2);
			}
		}
	}
}

void DlgProductSet::ckb_shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan2_clicked()
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	setConfig.shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan2 = ui->ckb_shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan_2->isChecked();
}
