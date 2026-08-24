#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "ImageProcessorModule.hpp"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <qfuture.h>
#include <qtconcurrentrun.h>
#include <QDateTime>
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

	// 将图像顺时针旋转 90 度（相机 2 为竖装，出图需转正后再进入图像处理）
	// 需要逆时针时将 ROTATE_90_CLOCKWISE 改为 ROTATE_90_COUNTERCLOCKWISE
	inline void RotateImage90Clockwise(cv::Mat& image)
	{
		if (image.empty()) {
			return;
		}
		// cv::rotate 内部为 transpose + flip，支持原地操作，无插值开销
		cv::rotate(image, image, cv::ROTATE_90_CLOCKWISE);
	}

	// 三个 PLC 循环写入功能的固定配置：每个功能占 60 个连续地址，仅写偶数地址（30 个槽位），
	// 清旧数据固定间隔 20 格（即写入当前槽位时，把 20 格之前的旧数据写 0 清除）
	constexpr int PLC_CIRCULAR_SLOT_COUNT = 30;		// 每个功能的写入槽位数（60 地址 / 步进 2）
	constexpr int PLC_CIRCULAR_CLEAR_INTERVAL = 20;	// 删除旧数据间隔（格数）
	constexpr int PLC_CIRCULAR_BASE_ADDR[3] = { 0, 60, 120 };	// 0=冷刀压痕 1=中心偏移值 2=切刀压痕

	// 一次循环写入的结果地址（-1 表示本次未写入，如 PLC 未连接）
	struct PlcCircularWriteResult
	{
		int writeAddress = -1;
		int clearAddress = -1;
	};

	// 每个图像处理线程独立的待检查写入 future 队列与帧计数（thread_local 按线程天然隔离，
	// 相机1/相机2 各自的多个处理线程互不干扰）
	thread_local std::vector<std::future<bool>> t_pendingPlcWriteFuts;
	thread_local int t_frameCountSincePlcCheck = 0;

	// 向 PLC 固定地址区间循环写入一个值：从 baseAddress 起按步进 2 共 slotCount 个槽位，
	// 每调用一次在当前槽位写 value 并推进一格，写满一轮回绕到起始；
	// 同时把"往前数 intervalSlots 格"的旧数据槽位写 0 清零。
	// 返回本次写入与清零的地址，PLC 未连接时不写入并返回 -1。
	inline PlcCircularWriteResult WritePlcCircular(int baseAddress, int slotCount, int intervalSlots, PlcCircularWriteState& state, uint16_t value)
	{
		PlcCircularWriteResult result;
		auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;
		if (!plcControllerScheduler) {
			qDebug() << "PLC disConnect!";
			return result;
		}
		if (baseAddress < 0 || slotCount <= 0 || intervalSlots < 0) {
			qWarning() << "PLC 循环写入参数非法，已跳过: base =" << baseAddress
				<< ", slotCount =" << slotCount << ", interval =" << intervalSlots;
			return result;
		}

		// 清零槽位 = 当前槽位往前数 intervalSlots 格（循环回绕；C++ 负数取模需二次修正）
		const int clearSlot = ((state.slot - intervalSlots) % slotCount + slotCount) % slotCount;
		const int clearAddress = baseAddress + clearSlot * 2;
		const int writeAddress = baseAddress + state.slot * 2;

		// 先入队清零、再入队写值（调度器同优先级 FIFO）：两槽相同时保证最终留下的是写入值
		auto clearFut = plcControllerScheduler->writeUInt16RegisterAsync(static_cast<uint16_t>(clearAddress), static_cast<uint16_t>(0));
		auto writeFut = plcControllerScheduler->writeUInt16RegisterAsync(static_cast<uint16_t>(writeAddress), value);

		// 不阻塞等待结果，攒入本线程待检查队列，由 MaybeAsyncCheckPlcWrites 每三次识别异步统一校验
		t_pendingPlcWriteFuts.push_back(std::move(clearFut));
		t_pendingPlcWriteFuts.push_back(std::move(writeFut));

		result.writeAddress = writeAddress;
		result.clearAddress = clearAddress;
		state.slot = (state.slot + 1) % slotCount;
		return result;
	}

	// 实时写入地址（每次拍照图像处理后写入，与上方的循环写入相互独立）
	constexpr int PLC_REALTIME_ADDR_LENGDAO = 200;		// 1相机压痕值（冷刀压痕）
	constexpr int PLC_REALTIME_ADDR_PIANYI = 202;		// 1相机中心偏移值
	constexpr int PLC_REALTIME_ADDR_QIEDAO = 204;		// 2相机压痕值（切刀压痕）
	constexpr int PLC_REALTIME_ADDR_WANCHENG = 206;		// 1相机写入完成标志（200与202都写完后写 1，PLC 读取后自行清零）
	constexpr int PLC_REALTIME_ADDR_WANCHENG2 = 208;	// 2相机写入状态标志（204写完后写 1，PLC 读取后自行清零）

	// 实时写入完成跟踪位：仅跟踪一相机的 200/202，两者自上一轮完成后都被写过时，向 206 写 1 并清零进入下一轮
	constexpr int PLC_REALTIME_BIT_LENGDAO = 0x1;
	constexpr int PLC_REALTIME_BIT_PIANYI = 0x2;
	constexpr int PLC_REALTIME_MASK_ALL = PLC_REALTIME_BIT_LENGDAO | PLC_REALTIME_BIT_PIANYI;
	std::atomic<int> g_plcRealtimeWriteMask{ 0 };	// 一相机多个处理线程共享

	// 向 PLC 实时写入地址写入一个值；返回值表示是否成功入队（PLC 未连接时返回 false），
	// 实际写入结果由 MaybeAsyncCheckPlcWrites 每三次识别异步统一校验
	inline bool WritePlcRealtimeValue(int address, uint16_t value)
	{
		auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;
		if (!plcControllerScheduler) {
			qDebug() << "PLC disConnect!";
			return false;
		}
		auto fut = plcControllerScheduler->writeUInt16RegisterAsync(static_cast<uint16_t>(address), value);

		// 不阻塞等待结果，攒入本线程待检查队列，由 MaybeAsyncCheckPlcWrites 异步统一校验
		t_pendingPlcWriteFuts.push_back(std::move(fut));
		return true;
	}

	// 每三次图像识别触发一次异步检查：取出本线程累计的写入 future，
	// 放到后台线程统一等待结果并统计失败数，避免阻塞图像处理线程
	inline void MaybeAsyncCheckPlcWrites()
	{
		++t_frameCountSincePlcCheck;
		if (t_frameCountSincePlcCheck < 3) {
			return;
		}
		t_frameCountSincePlcCheck = 0;

		if (t_pendingPlcWriteFuts.empty()) {
			return;
		}

		std::vector<std::future<bool>> futs;
		futs.swap(t_pendingPlcWriteFuts);

		QtConcurrent::run([futs = std::move(futs)]() mutable
			{
				int failCount = 0;
				for (auto& fut : futs) {
					if (!fut.get()) {
						++failCount;
					}
				}
				if (failCount > 0) {
					qWarning() << "PLC 写入异步检查:" << failCount << "/" << futs.size() << "条写入失败";
				}
			});
	}

	// 标记一相机某项实时数据已写入（须在对应数据写入入队之后调用）；当 200/202 两者
	// 自上一轮完成后都被写过时返回 true，由本次调用方负责向 206 写 1。
	// CAS 循环保证多线程并发下只有置齐最后一位的线程看到完成态，且标记清零原子完成
	inline bool MarkRealtimeWrittenAndTryComplete(int bit)
	{
		int mask = g_plcRealtimeWriteMask.load(std::memory_order_acquire);
		while (true) {
			const int newMask = mask | bit;
			const bool complete = (newMask == PLC_REALTIME_MASK_ALL);
			if (g_plcRealtimeWriteMask.compare_exchange_weak(mask, complete ? 0 : newMask, std::memory_order_acq_rel)) {
				return complete;
			}
		}
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

		// 按帧采集时刻的运行状态分发，而不是处理时刻的全局状态：
		// 调试切剔废时，切换窗口内到达的调试帧仍走 run_debug，不会被计数/写PLC
		auto captureState = frame.captureState;
		switch (captureState)
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
	// 相机 2 在 Debug 模式下同样先转正，保持与运行模式一致的画面方向与坐标系
	if (2 == imageProcessingModuleIndex) {
		RotateImage90Clockwise(frame.image);
	}

	auto& imgPro = *_imgProcess;
	imgPro(frame.image);
	auto maskImg = imgPro.getMaskImg(frame.image);
	auto defectResult = imgPro.getDefectResultInfo();

	emit imageReady(imageProcessingModuleIndex, QPixmap::fromImage(maskImg));

	rw::rqw::ImageInfo imageInfo(rw::rqw::cvMatToQImage(frame.image));
	save_image(imageInfo, maskImg, frame.captureState);
}

void ImageProcessor::run_OpenRemoveFunc(MatInfo& frame)
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	auto& imgPro = *_imgProcess;
	imgPro(frame.image);
	auto maskImg = imgPro.getMaskImg(frame.image);
	auto defectResult = imgPro.getDefectResultInfo();
	auto processResult = imgPro.getContext().getProcessResult();

	// 统计：每处理一帧拍照总量 +1（调试模式不进入此函数，不计数）
	auto& statisticalInfo = Modules::getInstance().runtimeInfoModule.statisticalInfo;
	++statisticalInfo.camera1PhotoCount;

	double width = 0.0;
	double centerDiffMm = 0.0;
	bool recognized = false;	// 本帧是否向 PLC 写入有效压痕宽度，未写入（写 0）则计入未识别总量

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
	}

	// 每张处理图都向固定地址区间（中心偏移值：60~119 中的偶数地址）循环写入一次中心点偏移值并推进槽位，无有效识别时写 0
	auto offsetWriteResult = WritePlcCircular(PLC_CIRCULAR_BASE_ADDR[1], PLC_CIRCULAR_SLOT_COUNT, PLC_CIRCULAR_CLEAR_INTERVAL,
		_plcOffsetWriteState,
		static_cast<uint16_t>(static_cast<int>(centerDiffMm * 100)));
	if (offsetWriteResult.writeAddress >= 0) {
		emit plcCircularWrite(1, offsetWriteResult.writeAddress, centerDiffMm, offsetWriteResult.clearAddress);
	}

	if (defectResult.defects.size() == 1)
	{
		if (imgPro.context().customFields.find("width") != imgPro.context().customFields.end())
		{
			auto pixToWorld = setConfig.xiangsudangliang1;
			width = std::any_cast<int>(imgPro.context().customFields["width"]) * pixToWorld;

			drawImg(maskImg, processResult, centerDiffMm);
			recognized = true;
		}
	}

	// 本帧未向 PLC 写入有效值（写 0），计入未识别总量
	if (!recognized)
	{
		++statisticalInfo.camera1UnrecognizedCount;
	}

	// 每张处理图都向固定地址区间（冷刀压痕：0~59 中的偶数地址）循环写入一次实测压痕宽度并推进槽位，无有效识别时写 0
	auto widthWriteResult = WritePlcCircular(PLC_CIRCULAR_BASE_ADDR[0], PLC_CIRCULAR_SLOT_COUNT, PLC_CIRCULAR_CLEAR_INTERVAL,
		_plcWidthWriteState,
		static_cast<uint16_t>(static_cast<int>(width * 100)));
	if (widthWriteResult.writeAddress >= 0) {
		emit plcCircularWrite(0, widthWriteResult.writeAddress, width, widthWriteResult.clearAddress);
	}

	// 实时写入：200=1相机压痕值（冷刀压痕）、202=1相机中心偏移值，未识别时写 0
	writePlcRealtime(PLC_REALTIME_ADDR_LENGDAO, PLC_REALTIME_BIT_LENGDAO, width);
	writePlcRealtime(PLC_REALTIME_ADDR_PIANYI, PLC_REALTIME_BIT_PIANYI, centerDiffMm);

	// 每三次识别异步检查一次本线程累计的 PLC 写入结果
	MaybeAsyncCheckPlcWrites();

	QStringList textList;
	textList.append("Z1实测压痕宽度:" + QString::number(width) + "mm");
	textList.append("Z2中心点偏差值:" + QString::number(centerDiffMm, 'f', 2) + "mm");
	std::vector<rw::imgPro::Color> colors;
	colors.push_back(rw::imgPro::Color::Blue);

	rw::imgPro::ImagePainter::drawTextOnImageWithFontSize(maskImg, textList, colors, 50);

	emit imageReady(frame.index, QPixmap::fromImage(maskImg));

	// 全部保存
	if (0 == setConfig.saveImgMode)
	{
		rw::rqw::ImageInfo imageInfo(rw::rqw::cvMatToQImage(frame.image));
		save_image(imageInfo, maskImg, frame.captureState);
	}
	// 只保存有识别到的
	else if (1 == setConfig.saveImgMode)
	{
		if (defectResult.disableDefects.size() > 0)
		{
			rw::rqw::ImageInfo imageInfo(rw::rqw::cvMatToQImage(frame.image));
			save_image(imageInfo, maskImg, frame.captureState);
		}
	}
}

void ImageProcessor::run_OpenRemoveFunc2(MatInfo& frame)
{
	// 相机 2 出图先顺时针旋转 90 度转正，后续处理、绘制与保存均基于转正后的图像
	RotateImage90Clockwise(frame.image);

	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	auto& imgPro = *_imgProcess;
	imgPro(frame.image);
	auto maskImg = imgPro.getMaskImg(frame.image);
	auto defectResult = imgPro.getDefectResultInfo();
	auto processResult = imgPro.getContext().getProcessResult();

	// 统计：每处理一帧拍照总量 +1（调试模式不进入此函数，不计数）
	auto& statisticalInfo = Modules::getInstance().runtimeInfoModule.statisticalInfo;
	++statisticalInfo.camera2PhotoCount;

	double width = 0.0;
	bool recognized = false;	// 本帧是否向 PLC 写入有效压痕宽度，未写入（写 0）则计入未识别总量

	if (defectResult.defects.size() == 1)
	{
		if (imgPro.context().customFields.find("width") != imgPro.context().customFields.end())
		{
			auto pixToWorld = setConfig.xiangsudangliang2;
			width = std::any_cast<int>(imgPro.context().customFields["width"]) * pixToWorld;
			drawImg(maskImg, processResult, 0.0);
			recognized = true;
		}
	}

	// 本帧未向 PLC 写入有效值（写 0），计入未识别总量
	if (!recognized)
	{
		++statisticalInfo.camera2UnrecognizedCount;
	}

	// 每张处理图都向固定地址区间（切刀压痕：120~179 中的偶数地址）循环写入一次实测压痕宽度并推进槽位，无有效识别时写 0
	auto widthWriteResult = WritePlcCircular(PLC_CIRCULAR_BASE_ADDR[2], PLC_CIRCULAR_SLOT_COUNT, PLC_CIRCULAR_CLEAR_INTERVAL,
		_plcWidthWriteState,
		static_cast<uint16_t>(static_cast<int>(width * 100)));
	if (widthWriteResult.writeAddress >= 0) {
		emit plcCircularWrite(2, widthWriteResult.writeAddress, width, widthWriteResult.clearAddress);
	}

	// 实时写入：204=2相机压痕值（切刀压痕），未识别时写 0；写入后置位 208 写入状态标志
	writePlcRealtimeQiedao(width);

	// 每三次识别异步检查一次本线程累计的 PLC 写入结果
	MaybeAsyncCheckPlcWrites();

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
		save_image(imageInfo, maskImg, frame.captureState);
	}
	// 只保存有识别到的
	else if (1 == setConfig.saveImgMode)
	{
		if (defectResult.disableDefects.size() > 0)
		{
			rw::rqw::ImageInfo imageInfo(rw::rqw::cvMatToQImage(frame.image));
			save_image(imageInfo, maskImg, frame.captureState);
		}
	}
}

void ImageProcessor::writePlcRealtime(int address, int bit, double valueMm)
{
	// 与循环写入同一约定：毫米值 *100 取整后按 uint16 发送（负的中心偏移值按补码环绕）
	if (!WritePlcRealtimeValue(address, static_cast<uint16_t>(static_cast<int>(valueMm * 100)))) {
		return;
	}
	emit plcRealtimeWrite(address, valueMm);

	// 数据写入已入队后再标记完成位；200/202 两者齐后向 206 写 1
	// （调度器同优先级 FIFO，保证 206 在 200/202 两条数据之后到达 PLC）
	if (MarkRealtimeWrittenAndTryComplete(bit)) {
		WritePlcRealtimeValue(PLC_REALTIME_ADDR_WANCHENG, 1);
		emit plcRealtimeWrite(PLC_REALTIME_ADDR_WANCHENG, 1.0);
	}
}

void ImageProcessor::writePlcRealtimeQiedao(double valueMm)
{
	// 204=2相机压痕值（切刀压痕），与循环写入同一约定：毫米值 *100 取整后按 uint16 发送
	if (!WritePlcRealtimeValue(PLC_REALTIME_ADDR_QIEDAO, static_cast<uint16_t>(static_cast<int>(valueMm * 100)))) {
		return;
	}
	emit plcRealtimeWrite(PLC_REALTIME_ADDR_QIEDAO, valueMm);

	// 204 写入入队后紧跟置位 208 写入状态标志（调度器同优先级 FIFO，保证 208 在 204 之后到达 PLC）
	WritePlcRealtimeValue(PLC_REALTIME_ADDR_WANCHENG2, 1);
	emit plcRealtimeWrite(PLC_REALTIME_ADDR_WANCHENG2, 1.0);
}

void ImageProcessor::save_image(rw::rqw::ImageInfo& imageInfo, const QImage& image, RunningState captureState)
{
	save_image_work(imageInfo, image, captureState);
}

void ImageProcessor::save_image_work(rw::rqw::ImageInfo& imageInfo, const QImage& image, RunningState captureState)
{
	auto& imageSaveEngine = Modules::getInstance().imgSaveModule.imageSaveEngine;
	auto& config = Modules::getInstance().configManagerModule.edgeWidthDetectionConfig;

	if (config.isSaveImg && imageSaveEngine)
	{
		// 文件名只保留时间戳：时分秒毫秒_年月日（classify 置空，不再拼接分类前缀）
		imageInfo.time = QDateTime::currentDateTime().toString("hhmmsszzz_yyyyMMdd");
		imageInfo.classify = "";
		// 一相机与二相机的存图分别保存到日期目录下的 Camera1 / Camera2 文件夹，
		// 其下再按 OK（原图）/ MASK（掩码图）分类
		const QString cameraDir = (2 == imageProcessingModuleIndex) ? "Camera2" : "Camera1";

		if (captureState == RunningState::OpenRemoveFunc)
		{
			imageInfo.dirName = cameraDir + "/OK";
			imageSaveEngine->pushImage(imageInfo);

			imageInfo.dirName = cameraDir + "/MASK";
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
	pen.setWidth(4);
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
	imageCenterPen.setWidth(2);
	imageCenterPen.setStyle(Qt::DashLine);
	painter.setPen(imageCenterPen);
	painter.drawLine(0, imageCenterY, qimage.width(), imageCenterY);

	// 绘制识别中心点水平线（红色虚线）
	if (!processResult.empty())
	{
		int detectionCenterY = processResult[0].center_y;
		QPen detCenterPen(QColor(255, 0, 0)); // 红色
		detCenterPen.setWidth(2);
		detCenterPen.setStyle(Qt::DashLine);
		painter.setPen(detCenterPen);
		painter.drawLine(0, detectionCenterY, qimage.width(), detectionCenterY);

		// 在识别中心点处绘制十字标记
		painter.drawLine(processResult[0].center_x - 8, detectionCenterY, processResult[0].center_x + 8, detectionCenterY);
		painter.drawLine(processResult[0].center_x, detectionCenterY - 8, processResult[0].center_x, detectionCenterY + 8);
	}

	painter.end();
}

void ImageProcessor::buildObbModelEngine(const QString& enginePath)
{
	rw::ModelEngineConfig modelEngineConfig;
	modelEngineConfig.conf_threshold = 0.1f;
	modelEngineConfig.nms_threshold = 0.01f;
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
		connect(processor, &ImageProcessor::plcCircularWrite, this, &ImageProcessingModule::plcCircularWrite, Qt::QueuedConnection);
		connect(processor, &ImageProcessor::plcRealtimeWrite, this, &ImageProcessingModule::plcRealtimeWrite, Qt::QueuedConnection);

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

	// 相机1图像在此回调处旋转180度，后续的检测线、文字绘制都基于旋转后的图像，保持正向
	if (1 == this->index) {
		cv::rotate(matInfo.mat, matInfo.mat, cv::ROTATE_180);
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
	// 打上帧到达瞬间的运行状态戳，处理线程按此分发（调试帧不会在切模式后被当作剔废帧计数）
	mat.captureState = Modules::getInstance().runtimeInfoModule.runningState.load();

	_queue.enqueue(mat);
	_condition.wakeOne();
}
