#pragma once

#include"ime_ModelEngineFactory.h"

#include <atomic>
#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <vector>
#include <QThread>
#include <QPixmap>
#include <imgPro_ImageProcess.hpp>
#include "rqw_CameraObjectCore.hpp"
#include "rqw_ImageSaveEngine.h"
#include "Utilty.hpp"


// PLC 循环写入状态：slot 为当前写入槽位下标（仅被所属处理线程访问）
struct PlcCircularWriteState
{
	int slot = 0;
};

class ImageProcessor : public QThread
{
	Q_OBJECT

public:
	ImageProcessor(QQueue<MatInfo>& queue,
		QMutex& mutex,
		QWaitCondition& condition,
		int workIndex,
		QObject* parent = nullptr);

protected:
	void run() override;

private:
	void run_debug(MatInfo& frame);				// 不开剔废时候的调试模式
private:
	void run_OpenRemoveFunc(MatInfo& frame);	// 开启剔废功能时的处理模式
	void run_OpenRemoveFunc2(MatInfo& frame);	// 相机2开启剔废时的处理（识别+算宽度+循环写入PLC，无剔废）
	// 向 PLC 实时写入地址写入一个值（毫米*100），并维护 206 一相机写入完成标志（200与202齐后写 1）
	void writePlcRealtime(int address, int bit, double valueMm);
	// 向 204 写入二相机压痕值（毫米*100），随后置位 208 二相机写入状态标志
	void writePlcRealtimeQiedao(double valueMm);
signals:
	void imageReady(size_t index, QPixmap image);
	// PLC 循环写入回显：funcIndex 0=冷刀压痕 1=中心偏移值 2=切刀压痕
	// writeAddress 为本次写入的地址，clearAddress 为本次清零的地址（-1 表示无）
	void plcCircularWrite(int funcIndex, int writeAddress, double value, int clearAddress);
	// PLC 实时写入回显：address 为写入的地址（200/202/204/206/208），value 为写入值
	void plcRealtimeWrite(int address, double value);
private:
	// 存图（captureState 为帧采集时刻的运行状态，决定是否按剔废模式分类保存）
	void save_image(rw::rqw::ImageInfo& imageInfo, const QImage& image, RunningState captureState);
	void save_image_work(rw::rqw::ImageInfo& imageInfo, const QImage& image, RunningState captureState);
private:
	// 在图像上面绘制短边(宽)
	void drawImg(QImage& qimage, const std::vector<rw::DetectionRectangleInfo>& processResult, double centerDiffMm);
private:
	std::unique_ptr<rw::imgPro::ImageProcess> _imgProcess;
public:
	// 构建模型引擎
	void buildObbModelEngine(const QString& enginePath);
private:
	QQueue<MatInfo>& _queue;
	QMutex& _mutex;
	QWaitCondition& _condition;
	int _workIndex;
	// PLC 循环写入游标（仅被本线程 run() 访问；相机2只使用宽度那一组）
	PlcCircularWriteState _plcWidthWriteState;
	PlcCircularWriteState _plcOffsetWriteState;
public:
	int imageProcessingModuleIndex;
};


class ImageProcessingModule : public QObject {
	Q_OBJECT
public:
	QString modelEnginePath;

public:
	// 初始化图像处理模块
	void BuildModule();
public:
	ImageProcessingModule(int numConsumers, QObject* parent = nullptr);

	~ImageProcessingModule();

public slots:
	// 相机回调函数
	void onFrameCaptured(rw::rqw::MatInfo matInfo, size_t index);

signals:
	void imageReady(size_t index, QPixmap image);
	void exposureStatsReady(double meanIntensity, double overRatio, double underRatio);
	// 转发自处理线程的 PLC 循环写入回显（供 UI 显示）
	void plcCircularWrite(int funcIndex, int writeAddress, double value, int clearAddress);
	// 转发自处理线程的 PLC 实时写入回显（供 UI 显示）
	void plcRealtimeWrite(int address, double value);

public:
	std::vector<ImageProcessor*> getProcessors() const {
		return _processors;
	}

private:
	QQueue<MatInfo> _queue;
	QMutex _mutex;
	QWaitCondition _condition;
	std::vector<ImageProcessor*> _processors;
	int _numConsumers;
	std::atomic<long long> _lastCamNs{ 0 };	// 防抖动时间戳（每模块独立，避免跨相机误丢帧）
public:
	size_t index;
};


