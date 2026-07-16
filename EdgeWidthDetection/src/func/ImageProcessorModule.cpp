#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "ImageProcessorModule.hpp"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <qfuture.h>
#include <qtconcurrentrun.h>
#include <atomic>
#include "Utilty.hpp"
#include <QPainter>
#include <QPen>
#include <cmath>
#include <algorithm>

#include "Modules.hpp"
#include "EdgeWidthDetection.h"

namespace {
	// 在给定最小间隔内只放行一次调用：成功返回 true，其他并发/过快的调用返回 false
	inline bool AllowOncePer(std::atomic<long long>& lastNs, std::chrono::nanoseconds minInterval)
	{
		using clock = std::chrono::steady_clock;
		const auto nowNs = std::chrono::duration_cast<std::chrono::nanoseconds>(
			clock::now().time_since_epoch()).count();

		auto prev = lastNs.load(std::memory_order_relaxed);
		if (nowNs - prev < minInterval.count())
			return false; // 距上次放行未到间隔，拒绝

		// 只有一个线程能成功更新 lastNs，其他并发线程会失败并返回 false
		return lastNs.compare_exchange_strong(prev, nowNs, std::memory_order_relaxed);
	}
} // namespace


ImageProcessor::ImageProcessor(QQueue<MatInfo>& queue, QMutex& mutex, QWaitCondition& condition, int workIndex, QObject* parent)
	: QThread(parent), _queue(queue), _mutex(mutex), _condition(condition), _workIndex(workIndex)
{

}

void ImageProcessor::run()
{
	while (!QThread::currentThread()->isInterruptionRequested()) {
		MatInfo frame;
		{
			QMutexLocker locker(&_mutex);
			if (_queue.isEmpty()) {
				_condition.wait(&_mutex);
				if (QThread::currentThread()->isInterruptionRequested()) {
					break;
				}
			}
			if (!_queue.isEmpty()) {
				frame = _queue.dequeue();
			}
			else {
				continue; // 如果队列仍为空，跳过本次循环
			}
		}

		// 检查 frame 是否有效
		if (frame.image.empty()) {
			continue; // 跳过空帧
		}

		auto currentRunningState = Modules::getInstance().runtimeInfoModule.runningState.load();
		switch (currentRunningState)
		{
		case RunningState::Debug:
			run_debug(frame);
			break;
		case RunningState::OpenRemoveFunc:
			if (2 == imageProcessingModuleIndex)
			{
				run_OpenRemoveFunc2(frame);
			}
			else
			{
				run_OpenRemoveFunc(frame);
			}
			break;
		default:
			break;
		}
	}
}

void ImageProcessor::run_debug(MatInfo& frame)
{
	auto& imgPro = *_imgProcess;
	imgPro(frame.image);
	auto maskImg = imgPro.getMaskImg(frame.image);
	auto defectResult = imgPro.getDefectResultInfo();

	emit imageReady(imageProcessingModuleIndex, QPixmap::fromImage(maskImg));

	rw::rqw::ImageInfo imageInfo(rw::rqw::cvMatToQImage(frame.image));
	save_image(imageInfo, maskImg);
}

void ImageProcessor::run_OpenRemoveFunc(MatInfo& frame)
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	auto& imgPro = *_imgProcess;
	imgPro(frame.image);
	auto maskImg = imgPro.getMaskImg(frame.image);
	auto defectResult = imgPro.getDefectResultInfo();
	auto processResult = imgPro.getContext().getProcessResult();

	double width = 0.0;
	double centerDiffMm = 0.0;

	// 计算识别中心点与图像中心点差值
	if (processResult.size() == 1)
	{
		auto pixToWorld = setConfig.xiangsudangliang1;
		int imageCenterY = frame.image.rows / 2;
		int detectionCenterY = processResult[0].center_y;
		double centerDiffPixel = imageCenterY - detectionCenterY;
		centerDiffMm = centerDiffPixel * pixToWorld;
		if (setConfig.shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan1)
		{
			centerDiffMm = -centerDiffMm;
		}

		// 写入中心点差值到PLC
		auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;
		if (plcControllerScheduler)
		{
			plcControllerScheduler->writeUInt16RegisterAsync(
				static_cast<uint16_t>(ModBusAddress::shibiezhongxindianyutuxiangzhongxindianchazhiAddress),
				static_cast<uint16_t>(centerDiffMm * 100));
		}
	}

	if (defectResult.defects.size() == 1)
	{
		if (imgPro.context().customFields.find("width") != imgPro.context().customFields.end())
		{
			auto pixToWorld = setConfig.xiangsudangliang1;
			width = std::any_cast<int>(imgPro.context().customFields["width"]) * pixToWorld;

			// 写入Plc
			writePlcController(width * 100);

			drawImg(maskImg, processResult, centerDiffMm);
		}
	}

	QStringList textList;
	textList.append("实测压痕宽度:" + QString::number(width) + "mm");
	textList.append("中心点偏差值:" + QString::number(centerDiffMm, 'f', 2) + "mm");
	std::vector<rw::imgPro::Color> colors;
	colors.push_back(rw::imgPro::Color::Blue);

	rw::imgPro::ImagePainter::drawTextOnImageWithFontSize(maskImg, textList, colors, 50);

	emit imageReady(frame.index, QPixmap::fromImage(maskImg));

	// 全部保存
	if (0 == setConfig.saveImgMode)
	{
		rw::rqw::ImageInfo imageInfo(rw::rqw::cvMatToQImage(frame.image));
		save_image(imageInfo, maskImg);
	}
	// 只保存有识别到的
	else if (1 == setConfig.saveImgMode)
	{
		if (defectResult.disableDefects.size() > 0)
		{
			rw::rqw::ImageInfo imageInfo(rw::rqw::cvMatToQImage(frame.image));
			save_image(imageInfo, maskImg);
		}
	}
}

void ImageProcessor::run_OpenRemoveFunc2(MatInfo& frame)
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	auto& imgPro = *_imgProcess;
	imgPro(frame.image);
	auto maskImg = imgPro.getMaskImg(frame.image);
	auto defectResult = imgPro.getDefectResultInfo();

	double width = 0.0;

	if (defectResult.defects.size() == 1)
	{
		if (imgPro.context().customFields.find("width") != imgPro.context().customFields.end())
		{
			auto pixToWorld = setConfig.xiangsudangliang2;
			width = std::any_cast<int>(imgPro.context().customFields["width"]) * pixToWorld;
		}
	}

	QStringList textList;
	textList.append("实测压痕宽度:" + QString::number(width) + "mm");
	std::vector<rw::imgPro::Color> colors;
	colors.push_back(rw::imgPro::Color::Blue);

	rw::imgPro::ImagePainter::drawTextOnImageWithFontSize(maskImg, textList, colors, 50);

	emit imageReady(frame.index, QPixmap::fromImage(maskImg));

	// 全部保存
	if (0 == setConfig.saveImgMode)
	{
		rw::rqw::ImageInfo imageInfo(rw::rqw::cvMatToQImage(frame.image));
		save_image(imageInfo, maskImg);
	}
	// 只保存有识别到的
	else if (1 == setConfig.saveImgMode)
	{
		if (defectResult.disableDefects.size() > 0)
		{
			rw::rqw::ImageInfo imageInfo(rw::rqw::cvMatToQImage(frame.image));
			save_image(imageInfo, maskImg);
		}
	}
}

void ImageProcessor::run_OpenRemoveFunc_emitErrorInfo(bool isbad)
{
	if (isbad)
	{
		if (1 == imageProcessingModuleIndex)
		{
			Modules::getInstance().eliminateModule.productPriorityQueue1->push(true);
			++Modules::getInstance().runtimeInfoModule.statisticalInfo.wasteCount;
		}
		else if (2 == imageProcessingModuleIndex)
		{
			++Modules::getInstance().runtimeInfoModule.statisticalInfo.wasteCount;
		}
	}
}

void ImageProcessor::save_image(rw::rqw::ImageInfo& imageInfo, const QImage& image)
{
	save_image_work(imageInfo, image);
}

void ImageProcessor::save_image_work(rw::rqw::ImageInfo& imageInfo, const QImage& image)
{
	auto& imageSaveEngine = Modules::getInstance().imgSaveModule.imageSaveEngine;
	auto& config = Modules::getInstance().configManagerModule.edgeWidthDetectionConfig;
	auto& runningState = Modules::getInstance().runtimeInfoModule.runningState;

	if (config.isSaveImg)
	{
		if (runningState == RunningState::OpenRemoveFunc)
		{
			imageInfo.classify = "OpenRemoveFuncNg";
			imageSaveEngine->pushImage(imageInfo);

			imageInfo.classify = "OpenRemoveFuncMask";
			imageInfo.image = image;
			imageSaveEngine->pushImage(imageInfo);
		}
		//else if (runningState == RunningState::Debug)
		//{
		//	imageInfo.classify = "Debug";
		//	imageSaveEngine->pushImage(imageInfo);
		//}
	}
}

void ImageProcessor::drawImg(QImage& qimage, const std::vector<rw::DetectionRectangleInfo>& processResult, double centerDiffMm)
{
	QPainter painter(&qimage);
	painter.setRenderHint(QPainter::Antialiasing, true);

	// 绘制 OBB 矩形长边
	QPen pen(QColor(0, 255, 0)); // 绿色
	pen.setWidth(2);
	painter.setPen(pen);

	for (const auto& result : processResult)
	{
		const QPoint lt(result.leftTop.first, result.leftTop.second);
		const QPoint rt(result.rightTop.first, result.rightTop.second);
		const QPoint lb(result.leftBottom.first, result.leftBottom.second);
		const QPoint rb(result.rightBottom.first, result.rightBottom.second);

		const double topLen = QLineF(lt, rt).length();
		const double bottomLen = QLineF(lb, rb).length();
		const double leftLen = QLineF(lt, lb).length();
		const double rightLen = QLineF(rt, rb).length();

		// 两组对边平均长度
		const double groupA = (topLen + bottomLen) * 0.5; // 上下
		const double groupB = (leftLen + rightLen) * 0.5; // 左右

		if (groupA >= groupB) {
			// 画长边：上、下
			painter.drawLine(lt, rt);
			painter.drawLine(lb, rb);
		}
		else {
			// 画长边：左、右
			painter.drawLine(lt, lb);
			painter.drawLine(rt, rb);
		}
	}

	// 绘制图像中心线（黄色虚线）
	int imageCenterY = qimage.height() / 2;
	QPen imageCenterPen(QColor(255, 255, 0)); // 黄色
	imageCenterPen.setWidth(1);
	imageCenterPen.setStyle(Qt::DashLine);
	painter.setPen(imageCenterPen);
	painter.drawLine(0, imageCenterY, qimage.width(), imageCenterY);

	// 绘制识别中心点水平线（红色虚线）
	if (!processResult.empty())
	{
		int detectionCenterY = processResult[0].center_y;
		QPen detCenterPen(QColor(255, 0, 0)); // 红色
		detCenterPen.setWidth(1);
		detCenterPen.setStyle(Qt::DashLine);
		painter.setPen(detCenterPen);
		painter.drawLine(0, detectionCenterY, qimage.width(), detectionCenterY);

		// 在识别中心点处绘制十字标记
		painter.drawLine(processResult[0].center_x - 8, detectionCenterY, processResult[0].center_x + 8, detectionCenterY);
		painter.drawLine(processResult[0].center_x, detectionCenterY - 8, processResult[0].center_x, detectionCenterY + 8);
	}

	painter.end();
}

void ImageProcessor::writePlcController(double width)
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;

	if (!plcControllerScheduler)
	{
		qDebug() << "PLC disConnect!";
		return;
	}

	plcControllerScheduler->writeUInt16RegisterAsync(static_cast<uint16_t>(ModBusAddress::shiceyahenkuanduAddress), static_cast<uint16_t>(width));
}

void ImageProcessor::buildObbModelEngine(const QString& enginePath)
{
	rw::ModelEngineConfig modelEngineConfig;
	modelEngineConfig.conf_threshold = 0.1f;
	modelEngineConfig.nms_threshold = 0.1f;
	modelEngineConfig.imagePretreatmentPolicy = rw::ImagePretreatmentPolicy::LetterBox;
	modelEngineConfig.letterBoxColor = cv::Scalar(114, 114, 114);
	modelEngineConfig.modelPath = enginePath.toStdString();
	auto engine = rw::ModelEngineFactory::createModelEngine(modelEngineConfig, rw::ModelType::Yolov11_Obb, rw::ModelEngineDeployType::TensorRT);

	_imgProcess = std::make_unique<rw::imgPro::ImageProcess>(engine);
	_imgProcess->context() = Modules::getInstance().imgProModule.imageProcessContext_PreProcess;
	_imgProcess->context().customFields["ImgProcessIndex"] = static_cast<int>(imageProcessingModuleIndex);
	_imgProcess->context().customFields["stationIdx"] = static_cast<int>(imageProcessingModuleIndex);
}

void ImageProcessingModule::BuildModule()
{
	for (int i = 0; i < _numConsumers; ++i) {
		static size_t workIndexCount = 0;
		ImageProcessor* processor = new ImageProcessor(_queue, _mutex, _condition, workIndexCount, this);
		workIndexCount++;
		processor->imageProcessingModuleIndex = index;
		processor->buildObbModelEngine(modelEnginePath);
		connect(processor, &ImageProcessor::imageReady, this, &ImageProcessingModule::imageReady, Qt::QueuedConnection);

		_processors.push_back(processor);
		processor->start();
	}
}

ImageProcessingModule::ImageProcessingModule(int numConsumers, QObject* parent)
	: QObject(parent), _numConsumers(numConsumers)
{

}

ImageProcessingModule::~ImageProcessingModule()
{
	// 通知所有线程退出
	for (auto processor : _processors) {
		processor->requestInterruption();
	}

	// 唤醒所有等待的线程
	{
		QMutexLocker locker(&_mutex);
		_condition.wakeAll();
	}

	// 等待所有线程退出
	for (auto processor : _processors) {
		if (processor->isRunning()) {
			processor->wait(1000); // 使用超时机制，等待1秒
		}
		delete processor;
	}
}

void ImageProcessingModule::onFrameCaptured(rw::rqw::MatInfo matInfo, size_t index)
{
	// 防抖动处理
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	const long long debounceMs = static_cast<long long>(std::max(0.0, setConfig.xiangjiguangdianpingbishijian));
	const auto minInterval = std::chrono::milliseconds(debounceMs);

	if (!AllowOncePer(_lastCamNs, minInterval)) {
		return;
	}

	if (matInfo.mat.channels() == 4) {
		cv::cvtColor(matInfo.mat, matInfo.mat, cv::COLOR_BGRA2BGR);
	}
	if (matInfo.mat.type() != CV_8UC3) {
		matInfo.mat.convertTo(matInfo.mat, CV_8UC3);
	}

	if (matInfo.mat.empty()) {
		return; // 跳过空帧
	}

	// 计算整图亮度统计，用于自动曝光
	auto& cfg = Modules::getInstance().configManagerModule.setConfig;
	bool autoExposureOn = (1 == this->index) ? cfg.autoExposureEnabled1 : cfg.autoExposureEnabled2;
	if (autoExposureOn) {
		cv::Mat gray;
		cv::cvtColor(matInfo.mat, gray, cv::COLOR_BGR2GRAY);
		double meanIntensity = cv::mean(gray)[0];

		cv::Mat overMask;
		cv::Mat underMask;
		cv::threshold(gray, overMask, cfg.autoExposureOverExposeThreshold, 255, cv::THRESH_BINARY);
		cv::threshold(gray, underMask, cfg.autoExposureUnderExposeThreshold, 255, cv::THRESH_BINARY_INV);

		double totalPixels = static_cast<double>(gray.total());
		double overRatio = (totalPixels > 0.0) ? (cv::countNonZero(overMask) / totalPixels) : 0.0;
		double underRatio = (totalPixels > 0.0) ? (cv::countNonZero(underMask) / totalPixels) : 0.0;

		emit exposureStatsReady(meanIntensity, overRatio, underRatio);
	}

	QMutexLocker locker(&_mutex);
	// 队列最多只有一张
	if (_queue.size() >= 1)
	{
		return;
	}
	MatInfo mat;
	mat.image = matInfo.mat;
	mat.index = index;

	_queue.enqueue(mat);
	_condition.wakeOne();
}
