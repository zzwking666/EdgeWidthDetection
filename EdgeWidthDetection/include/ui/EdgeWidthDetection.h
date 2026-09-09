#pragma once

#include <QMainWindow>
#include <memory>
#include <atomic>
#include <QCheckBox>
#include <QHash>
#include <QTimer>
#include <QVector>

#include "rqw_LabelClickable.h"
#include "DlgCloseForm.h"
#include "oso_StorageContext.hpp"
#include "rqw_RunEnvCheck.hpp"
#include "PictureViewerThumbnails.h"

QT_BEGIN_NAMESPACE
namespace Ui { class EdgeWidthDetectionClass; };
QT_END_NAMESPACE

class QLabel;

class EdgeWidthDetection : public QMainWindow
{
	Q_OBJECT

public:
	EdgeWidthDetection(QWidget* parent = nullptr);
	~EdgeWidthDetection();
#ifdef BUILD_WITHOUT_HARDWARE
public:
	QCheckBox* cBox_testPushImg{ nullptr };
public slots:
	void cBox_testPushImg_checked(bool checked);
#endif
public:
	void build_ui();
	void build_connect();
	void build_EdgeWidthDetectionData();
	void ini_clickableTitle();
	void build_DlgCloseForm();
public:
	void initializeComponents();
public:
	void build_camera();
public:
	void build_plcController();
public slots:
	void updateCameraLabelState(int cameraIndex, bool state);

	void onUpdateStatisticalInfoUI();

	void onUpdatePLCWarnningInfoUI(uint16_t warnningInfo);

	void onCameraDisplay(size_t index, QPixmap image);

	void onAutoExposureInfo(double targetExposure, double meanIntensity,
		double overRatio, double underRatio);

	void onAutoExposureInfo2(double targetExposure, double meanIntensity,
		double overRatio, double underRatio);

	void lb_title_clicked();
signals:
	void autoExposureToggled(bool enabled);
	void autoExposureToggled2(bool enabled);
private slots:
	void pbtn_exit_clicked();
	void pbtn_set_clicked();
	void pbtn_Modbus_clicked();
	void rbtn_debug_checked(bool checked);
	void rbtn_removeFunc_checked(bool checked);
	void pbtn_resetProduct_clicked();
	void pbtn_openSaveLocation_clicked();

	void rbtn_ruoguang_checked(bool checked);
	void rbtn_zhongguang_checked(bool checked);
	void rbtn_qiangguang_checked(bool checked);

	void ckb_saveImg_checked(bool checked);
	void ckb_autoExposure_checked(bool checked);

	void rbtn_ruoguang_2_checked(bool checked);
	void rbtn_zhongguang_2_checked(bool checked);
	void rbtn_qiangguang_2_checked(bool checked);
	void ckb_autoExposure_2_checked(bool checked);

	// 主界面拍照面板按钮（点位地址以 modbus_main.csv 为准）
	void pbtn_start_clicked();			// 启动：开关式点动，写 1 置绿 / 再点写 0 恢复原样式
	void pbtn_stop_clicked();			// 停止：同上
	void pbtn_cutCompensate_clicked();	// 切刀补偿切换：按当前状态取反写入
	void onMainUiRefreshTimeout();		// 定时轮询主界面面板全部点位并刷新显示

public:
	// 公开给 CSV 解析辅助函数使用
	enum class MainPointType { Float, Dint, Bool };

private:
	void refreshExposureInfo();
	void updateExposureInfoVisibility();

	// 主界面拍照/切刀面板点位：由 config/modbus_main.csv 定义（名称,类型,协议地址,读写），
	// 与 DlgModbus 使用的 modbus.csv 相互独立（相当于单独的 sheet），名称列必须是约定的点位名
	struct MainUiPoint
	{
		QString name;				// 点位名（与面板上哪一行对应由名称决定）
		MainPointType type{ MainPointType::Float };
		int protocolAddress{ 0 };	// Modbus 协议地址（实际读写报文使用）
		bool writable{ false };
	};

	void build_mainUiModbus();		// 加载面板点位、改造可点击数值标签、连接按钮、启动轮询
	void loadMainUiPoints();		// 从 modbus_main.csv 加载；文件缺失时生成默认表，非法时弹窗并退出程序
	[[noreturn]] void abortOnMainUiLoadErrors(const QStringList& errors);
	void saveMainUiPoints();		// 将默认点位表写入 modbus_main.csv（仅文件缺失时调用）

	const MainUiPoint* findMainUiPoint(const QString& name) const;
	bool checkManualWriteReady();	// 手动写入前置检查：总开关已开 + PLC 已连接，不满足时弹窗并返回 false
	bool writeMainCoil(const QString& pointName, bool value);		// 按点位名写线圈（启动/停止/切刀补偿），返回是否写入成功
	void writeMainValue(const QString& pointName);					// 按点位名弹数字键盘写数值（四个可写速度/长度）

	// 将普通 QLabel 原位替换为可点击标签（点击写数值用），样式与 replaceWidget 用法同 ini_clickableTitle
	rw::rqw::ClickableLabel* replaceWithClickableValue(QLabel* oldLabel);
	void updateCutCompensateButton();	// 按 _cutCompensateOn 刷新补偿按钮的文字与背景色（开=绿/关=红）

	// 根据相机/PLC连接状态刷新标题栏背景色（任一未连接则置红）
	void updateHeadBackground();

	// 各设备连接状态缓存，用于决定标题栏背景色
	bool _camera1Connected{ true };
	bool _camera2Connected{ true };
	bool _plcConnected{ true };

	rw::rqw::ClickableLabel* clickableTitle = nullptr;
	DlgCloseForm* _dlgCloseForm = nullptr;
	PictureViewerThumbnails* _picturesViewer = nullptr;

	// 主界面面板点位数据与控件映射
	QVector<MainUiPoint> _mainUiPoints;
	QHash<QString, QLabel*> _mainUiValueLabels;		// 数值行：点位名 -> 当前值标签
	QTimer _mainUiRefreshTimer;						// 面板点位轮询定时器
	bool _mainUiRefreshInFlight{ false };			// 上一次轮询未结束时跳过本次
	bool _cutCompensateOn{ false };					// 切刀补偿当前状态（轮询更新，切换按钮据此取反）
	bool _startOn{ false };							// 「启动」按钮当前状态（本地记录，点击时取反写入）
	bool _stopOn{ false };							// 「停止」按钮当前状态
	QString _pbtnStartDefaultStyle;					// 启动按钮在 .ui 中的原始样式表（关状态时恢复）
	QString _pbtnStopDefaultStyle;					// 停止按钮在 .ui 中的原始样式表

	// 四个可写数值行的可点击标签（点击弹出数字键盘写入）
	rw::rqw::ClickableLabel* clk_setPhotoLength{ nullptr };
	rw::rqw::ClickableLabel* clk_cutJogContSpeed{ nullptr };
	rw::rqw::ClickableLabel* clk_cutJogOnceSpeed{ nullptr };
	rw::rqw::ClickableLabel* clk_autoSpeed{ nullptr };
private:
	Ui::EdgeWidthDetectionClass* ui;
	int minimizeCount{ 3 };
	QString _cam1ExposureInfo;
	QString _cam2ExposureInfo;
};