#pragma once

#include <QDialog>


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

	void cbox_registerAttribute_clicked();
	void btn_buchangsuduxierudizhi_clicked();
	void btn_buchangliangxierudizhi_clicked();
	void btn_huodePLCbaojingxinxidizhi_clicked();
public:
	Ui::DlgProductSetClass* ui;
};

