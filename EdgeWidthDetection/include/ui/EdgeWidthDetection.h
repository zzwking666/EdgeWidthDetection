#pragma once

#include <QMainWindow>
#include <memory>
#include <atomic>
#include <QCheckBox>

#include "rqw_LabelClickable.h"
#include "DlgCloseForm.h"
#include "oso_StorageContext.hpp"
#include "rqw_RunEnvCheck.hpp"
#include "PictureViewerThumbnails.h"

QT_BEGIN_NAMESPACE
namespace Ui { class EdgeWidthDetectionClass; };
QT_END_NAMESPACE

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

private:
	void refreshExposureInfo();
	void updateExposureInfoVisibility();

	// 根据相机/PLC连接状态刷新标题栏背景色（任一未连接则置红）
	void updateHeadBackground();

	// 各设备连接状态缓存，用于决定标题栏背景色
	bool _camera1Connected{ true };
	bool _camera2Connected{ true };
	bool _plcConnected{ true };

	rw::rqw::ClickableLabel* clickableTitle = nullptr;
	DlgCloseForm* _dlgCloseForm = nullptr;
	PictureViewerThumbnails* _picturesViewer = nullptr;
private:
	Ui::EdgeWidthDetectionClass* ui;
	int minimizeCount{ 3 };
	QString _cam1ExposureInfo;
	QString _cam2ExposureInfo;
};