#pragma once

#include <QDialog>

#include "Utilty.hpp"


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
	void build_connect();

signals:
	void paramsChanged();
public slots:
	void onUpdatePLCInfo(QVector<PlcReadItem> datas);
private slots:
	void btn_close_clicked();

	void btn_xiangjiguangdianpingbishijian_clicked();
	void btn_shuchuxinhaoyanshi_clicked();
	void btn_shuchuxinhaochixushijian_clicked();
	void cbox_changeSaveImgMode_clicked();
	void btn_score_clicked();
	void ckb_autoSaveImg_clicked();

	void btn_testTrigger1_1_clicked();
	void btn_testTrigger2_1_clicked();

	void btn_shangxianwei1_clicked();
	void btn_xiaxianwei1_clicked();
	void btn_zuoxianwei1_clicked();
	void btn_youxianwei1_clicked();
	void btn_zengyi_clicked();
	void btn_xiangsudangliang1_clicked();
	void btn_debugFrame_clicked();
	void btn_openRemoveFrame_clicked();

	void btn_qiangguang_clicked();
	void btn_zhongguang_clicked();
	void btn_ruoguang_clicked();

	// 读取PLC
	void btn_shicekuanduduqudizhi_clicked();
	void btn_shedingbiaozhunzhiduqudizhi_clicked();
	void btn_changdujiangeduqudizhi_clicked();
	void btn_paizhaochangdujiangeduqudizhi_clicked();
	void btn_bujinyiquanmaichongshuduqudizhi_clicked();
	void btn_luojuduqudizhi_clicked();
	void btn_huodePLCbaojingxinxidizhi_clicked();
	// 写入PLC
	void btn_shicekuanduxierushuzhi_clicked();
	void btn_shedingbiaozhunzhixierushuzhi_clicked();
	void btn_changdujiangexierushuzhi_clicked();
	void btn_paizhaochangdujiangexierushuzhi_clicked();
	void btn_bujinyiquanmaichongshuxierushuzhi_clicked();
	void btn_luojuxierushuzhi_clicked();

	void btn_writeshicekuandu_clicked();
	void btn_writeshedingbiaozhunzhi_clicked();
	void btn_writechangdujiange_clicked();
	void btn_writepaizhaochangdujiange_clicked();
	void btn_writebujinyiquanmaichongshu_clicked();
	void btn_writeluojuxierushuzhi_clicked();

	void tabWidget_indexChanged(int index);
private:
	bool checkIsPLCAddressSame(int newAddress, const QString& currentKey);
public:
	Ui::DlgProductSetClass* ui;
};

