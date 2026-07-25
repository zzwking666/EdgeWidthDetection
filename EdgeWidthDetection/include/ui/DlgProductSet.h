#pragma once

#include <QCloseEvent>
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
	void save_config();
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

	// 设置与读取地址
	void btn_shiceyahenkuanduduqudizhi_clicked();
	void btn_shedingyahenbiaozhunzhiduqudizhi_clicked();
	void btn_paizhaojiangejuliduqudizhi_clicked();
	void btn_daizichangdubiaozhunduqudizhi_clicked();
	void btn_jimiqiyiquanmaichongshuduqudizhi_clicked();
	void btn_jimiqiyiquanzhouchangduqudizhi_clicked();
	void btn_bujinyiquanmaichongshuduqudizhi_clicked();

	void btn_bujinluojuduqudizhi_clicked();
	void btn_daoyidongbuchangduqudizhi_clicked();
	void btn_zidongdaoyidongzuidajuliduqudizhi_clicked();
	void btn_daojiakeyidongdezuidajuliduqudizhi_clicked();
	void btn_daoyidongsudumaichongpinlvduqudizhi_clicked();

	void btn_huodePLCbaojingxinxidizhi_clicked();
	void btn_keyipaizhaoxinhaodizhi_clicked();
	void btn_tingzhipaizhaoxinhaodizhi_clicked();
	void btn_daizishicechangdudizhi_clicked();
	void btn_shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhi_clicked();

	// 循环写入地址（一相机宽度/偏移、二相机宽度：起始/末尾/删除旧数据间隔）
	void btn_shiceyahenkuanduxieruqishidizhi1_clicked();
	void btn_shiceyahenkuanduxierumoweidizhi1_clicked();
	void btn_shiceyahenkuandushanchujiushujujiange1_clicked();
	void btn_tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixieruqishidizhi1_clicked();
	void btn_tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixierumoweidizhi1_clicked();
	void btn_tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhishanchujiushujujiange1_clicked();
	void btn_shiceyahenkuanduxieruqishidizhi2_clicked();
	void btn_shiceyahenkuanduxierumoweidizhi2_clicked();
	void btn_shiceyahenkuandushanchujiushujujiange2_clicked();

	// 写入PLC
	void btn_shiceyahenkuanduxierushuzhi_clicked();
	void btn_shedingyahenbiaozhunkuanduzhixierushuzhi_clicked();
	void btn_paizhaojiangejulixierushuzhi_clicked();
	void btn_daizichangdubiaozhunxierushuzhi_clicked();
	void btn_jimiqiyiquanmaichongshuxierushuzhi_clicked();
	void btn_jimiqiyiquanzhouchangxierushuzhi_clicked();
	void btn_bujinyiquanmaichongshuxierushuzhi_clicked();
	void btn_bujinluojuxierushuzhi_clicked();
	void btn_daoyidongbuchangxierushuzhi_clicked();
	void btn_zidongdaoyidongzuidajulixierushuzhi_clicked();
	void btn_daojiakeyidongdezuidajulixierushuzhi_clicked();
	void btn_daoyidongsudumaichongpinlvxierushuzhi_clicked();
	void btn_shibiezhongxindianyutuxiangzhongxindianchazhixierushuzhi_clicked();

	void btn_writeshiceyahenkuanduxierushuzhi_clicked();
	void btn_writeshedingyahenbiaozhunkuanduzhixierushuzhi_clicked();
	void btn_writepaizhaojiangejulixierushuzhi_clicked();
	void btn_writedaizichangdubiaozhunxierushuzhi_clicked();
	void btn_writejimiqiyiquanmaichongshuxierushuzhi_clicked();
	void btn_writejimiqiyiquanzhouchangxierushuzhi_clicked();
	void btn_writebujinyiquanmaichongshuxierushuzhi_clicked();
	void btn_writebujinluojuxierushuzhi_clicked();
	void btn_writedaoyidongbuchangxierushuzhi_clicked();
	void btn_writezidongdaoyidongzuidajulixierushuzhi_clicked();
	void btn_writedaojiakeyidongdezuidajulixierushuzhi_clicked();
	void btn_writedaoyidongsudumaichongpinlvxierushuzhi_clicked();
	void btn_writeshibiezhongxindianyutuxiangzhongxindianchazhixierushuzhi_clicked();

	void tabWidget_indexChanged(int index);
private:
	bool checkIsPLCAddressSame(int newAddress, const QString& currentKey);
protected:
	void closeEvent(QCloseEvent* event) override;
public:
	Ui::DlgProductSetClass* ui;
};

