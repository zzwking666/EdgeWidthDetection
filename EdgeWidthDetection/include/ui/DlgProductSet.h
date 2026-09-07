#pragma once

#include <QCloseEvent>
#include <QDialog>
#include <QPushButton>
#include <QVector>


QT_BEGIN_NAMESPACE
namespace Ui { class DlgProductSetClass; };
QT_END_NAMESPACE

class DlgProductSet : public QDialog
{
	Q_OBJECT

public:
	DlgProductSet(QWidget* parent = nullptr);
	~DlgProductSet();

public:
	void build_ui();
	void read_config();
	void save_config();
	void build_connect();

signals:
	void paramsChanged();
public slots:
	// PLC 循环写入回显：funcIndex 0=冷刀压痕 1=中心偏移值 2=切刀压痕
	// writeAddress 为本次写入的地址，clearAddress 为本次清零的地址（-1 表示无）
	void onPlcCircularWrite(int funcIndex, int writeAddress, double value, int clearAddress);
	// PLC 实时写入回显：address 为写入的地址（200/202/204/206/208），value 为写入值
	void onPlcRealtimeWrite(int address, double value);
private slots:
	void btn_close_clicked();

	void btn_xiangjiguangdianpingbishijian_clicked();
	void btn_shuchuxinhaoyanshi_clicked();
	void btn_shuchuxinhaochixushijian_clicked();
	void cbox_changeSaveImgMode_clicked();
	void btn_score_clicked();
	void ckb_autoSaveImg_clicked();
	void ckb_shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan_clicked();

	void btn_testTrigger1_1_clicked();
	void btn_testTrigger2_1_clicked();

	void btn_shangxianwei1_clicked();
	void btn_xiaxianwei1_clicked();
	void btn_zuoxianwei1_clicked();
	void btn_youxianwei1_clicked();
	void btn_zengyi1_clicked();
	void btn_xiangsudangliang1_clicked();
	void btn_debugFrame1_clicked();
	void btn_openRemoveFrame1_clicked();

	void btn_qiangguang1_clicked();
	void btn_zhongguang1_clicked();
	void btn_ruoguang1_clicked();

	void btn_testTrigger1_2_clicked();
	void btn_testTrigger2_2_clicked();

	void btn_shangxianwei2_clicked();
	void btn_xiaxianwei2_clicked();
	void btn_zuoxianwei2_clicked();
	void btn_youxianwei2_clicked();
	void btn_zengyi2_clicked();
	void btn_xiangsudangliang2_clicked();
	void btn_debugFrame2_clicked();
	void btn_openRemoveFrame2_clicked();

	void btn_qiangguang2_clicked();
	void btn_zhongguang2_clicked();
	void btn_ruoguang2_clicked();

	void ckb_shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan2_clicked();

	void tabWidget_indexChanged(int index);
	// 测试写入：向 0~179 每个地址写入其地址值（一相机与二相机地址共用此按钮）
	void btn_testPlcAddressWrite_clicked();
private:
	// 在"一相机地址"/"二相机地址"页中构建固定 60 地址的循环写入显示区（按钮 + 地址标签）
	void buildCircularWriteUi();
private:
	// 三个功能的写入值显示按钮：0=冷刀压痕(0~59) 1=中心偏移值(60~119) 2=切刀压痕(120~179)
	QVector<QPushButton*> _plcCircularButtons[3];
protected:
	void closeEvent(QCloseEvent* event) override;
public:
	Ui::DlgProductSetClass* ui;
};

