#include "ImgSaveModule.hpp"

#include <QDate>
#include <QDateTime>
#include <QDir>
#include <QStorageInfo>
#include <QTimer>
#include <QtConcurrent/QtConcurrent>

#include "Utilty.hpp"

void ImgSaveModule::build()
{
	// 异步清理超期的历史图像文件夹，避免旧文件过多（如大压缩包）时阻塞启动流程
	startCleanupOldFoldersAsync();

	imageSaveEngine = std::make_unique<rw::rqw::ImageSaveEngine>(this, cimgSaveThreadNum);

	imageSaveEngine->setSaveImgFormat(rw::rqw::ImageSaveFormat::JPEG);
	imageSaveEngine->setSaveImgQuality(cimgSaveQuality);
	QDir dir;
	QString imageSavePath = globalPath.imageSaveRootPath;

	QString currentDate = QDate::currentDate().toString("yyyy_MM_dd");
	QString imageSaveEnginePath = imageSavePath + currentDate;

	QString imagesFilePathFilePathFull = dir.absoluteFilePath(imageSaveEnginePath);
	imageSaveEngine->setRootPath(imagesFilePathFilePathFull);
	imageSaveEngine->setSavePolicy(rw::rqw::ImageSaveEnginePolicy::Normal);

	// 启动时先立即检测一次磁盘空间，之后每 10 分钟定时检测
	checkDiskSpace();
	_diskCheckTimer = new QTimer(this);
	connect(_diskCheckTimer, &QTimer::timeout, this, &ImgSaveModule::checkDiskSpace);
	_diskCheckTimer->start(cdiskCheckIntervalMs);

	// 初始化出图时间戳并启动停机检测：相机 10 秒无出图视为停机，停机后自动清理磁盘空间
	_lastFrameMs.store(QDateTime::currentMSecsSinceEpoch());
	_idleCleanupCancel->store(false);
	_idleCheckTimer = new QTimer(this);
	connect(_idleCheckTimer, &QTimer::timeout, this, &ImgSaveModule::checkIdleCleanup);
	_idleCheckTimer->start(cidleCheckIntervalMs);
}

void ImgSaveModule::destroy()
{
	// 停止磁盘空间定时检测
	if (_diskCheckTimer) {
		_diskCheckTimer->stop();
	}
	// 停止停机检测定时器
	if (_idleCheckTimer) {
		_idleCheckTimer->stop();
	}

	// 取消后台清理并等待其退出（取消以单个文件夹为粒度生效，正在删除的文件夹会删完才退出）
	_cleanupCancel->store(true);
	if (_cleanupFuture.isStarted() && !_cleanupFuture.isFinished()) {
		_cleanupFuture.waitForFinished();
	}
	// 取消停机磁盘清理并等待其退出
	_idleCleanupCancel->store(true);
	if (_idleCleanupFuture.isStarted() && !_idleCleanupFuture.isFinished()) {
		_idleCleanupFuture.waitForFinished();
	}

	if (imageSaveEngine)
	{
		imageSaveEngine.reset();
	}
}

void ImgSaveModule::startCleanupOldFoldersAsync()
{
	const QString imageSavePath = globalPath.imageSaveRootPath;
	// cancel 标志通过 shared_ptr 传给后台任务，即使模块已析构后台线程也不会访问悬空指针
	auto cancelFlag = _cleanupCancel;
	cancelFlag->store(false);

	_cleanupFuture = QtConcurrent::run([imageSavePath, cancelFlag]() {
		QDir dir(imageSavePath);
		if (!dir.exists()) {
			qWarning() << "[ImgSave] 图像保存根目录不存在，跳过历史图像清理:" << imageSavePath;
			return;
		}

		// 设置过滤器，只获取文件夹
		dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
		const QFileInfoList folderList = dir.entryInfoList();

		// 解析出名称可识别为日期的文件夹并按日期升序排序
		QVector<QPair<QDate, QString>> dateFolderPairs;
		for (const QFileInfo& folderInfo : folderList) {
			const QDate folderDate = QDate::fromString(folderInfo.fileName(), "yyyy_MM_dd");
			if (folderDate.isValid()) {
				dateFolderPairs.append(qMakePair(folderDate, folderInfo.fileName()));
			}
		}
		std::sort(dateFolderPairs.begin(), dateFolderPairs.end(),
			[](const QPair<QDate, QString>& a, const QPair<QDate, QString>& b) {
				return a.first < b.first;
			});

		// 从最旧的开始删除超过 7 天的文件夹
		const QDate currentDate = QDate::currentDate();
		for (const auto& pair : dateFolderPairs) {
			if (cancelFlag->load()) {
				qInfo() << "[ImgSave] 历史图像清理已取消";
				return;
			}
			if (pair.first >= currentDate.addDays(-7)) {
				break;	// 已按日期升序排列，后续文件夹均未超期
			}
			const QString folderPath = imageSavePath + pair.second;
			qInfo() << "[ImgSave] 正在删除超期图像文件夹:" << folderPath;
			if (!QDir(folderPath).removeRecursively()) {
				qWarning() << "[ImgSave] 删除失败（文件可能被占用）:" << folderPath;
			}
		}
		qInfo() << "[ImgSave] 历史图像清理完成";
	});
}

void ImgSaveModule::checkDiskSpace()
{
	QString rootPath;
	if (imageSaveEngine) {
		rootPath = imageSaveEngine->getRootPath();
	}
	if (rootPath.isEmpty()) {
		rootPath = globalPath.imageSaveRootPath;
	}

	QStorageInfo storage(rootPath);
	if (!storage.isValid() || !storage.isReady()) {
		// 磁盘信息暂不可用时保持当前状态，下次检测再更新
		return;
	}

	const qint64 freeBytes = storage.bytesAvailable();
	const bool enough = freeBytes >= cminFreeDiskBytes;
	const double freeGB = static_cast<double>(freeBytes) / (1024.0 * 1024.0 * 1024.0);
	_lastFreeGB.store(freeGB);

	// 仅在状态发生变化时输出日志并通知 UI，避免每 10 分钟刷屏/弹窗
	if (_diskSpaceEnough.exchange(enough) != enough) {
		if (enough) {
			qInfo() << "[ImgSave] 存图磁盘剩余空间恢复:" << QString::number(freeGB, 'f', 2)
				<< "GB，已恢复存图";
		}
		else {
			qWarning() << "[ImgSave] 存图磁盘剩余空间不足:" << QString::number(freeGB, 'f', 2)
				<< "GB（低于 10GB），已暂停存图";
		}
		emit diskSpaceStateChanged(enough, freeGB);
	}
}

void ImgSaveModule::notifyFrameActivity()
{
	_lastFrameMs.store(QDateTime::currentMSecsSinceEpoch());
	// 恢复出图后复位，下一轮停机允许再次清理
	_idleCleanupDone.store(false);
}

void ImgSaveModule::checkIdleCleanup()
{
	const qint64 lastFrameMs = _lastFrameMs.load();
	if (lastFrameMs <= 0) {
		return;
	}
	// 相机连续 10 秒无出图视为设备停机
	if (QDateTime::currentMSecsSinceEpoch() - lastFrameMs < ccameraIdleThresholdMs) {
		return;
	}
	if (_idleCleanupDone.load() || _idleCleanupRunning.load()) {
		return;	// 本轮停机已清理过，或上一次清理尚未结束
	}
	_idleCleanupDone.store(true);
	qInfo() << "[ImgSave] 检测到设备停机（相机超过 10 秒无出图），开始检查存图磁盘空间";
	startIdleDiskCleanupAsync();
}

void ImgSaveModule::startIdleDiskCleanupAsync()
{
	const QString imageSavePath = globalPath.imageSaveRootPath;
	auto cancelFlag = _idleCleanupCancel;
	_idleCleanupRunning.store(true);

	_idleCleanupFuture = QtConcurrent::run([this, imageSavePath, cancelFlag]() {
		const QString todayFolder = QDate::currentDate().toString("yyyy_MM_dd");

		while (!cancelFlag->load()) {
			QStorageInfo storage(imageSavePath);
			if (!storage.isValid() || !storage.isReady()) {
				break;	// 磁盘信息暂不可用，留待下次停机再试
			}
			if (storage.bytesAvailable() >= cminFreeDiskBytes) {
				break;	// 剩余空间已满足存图条件
			}

			// 找出最早的日期存图文件夹（排除当天目录，当天目录正在写入不能删）
			QDir dir(imageSavePath);
			dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
			const QFileInfoList folderList = dir.entryInfoList();
			QDate oldestDate;
			QString oldestFolder;
			for (const QFileInfo& folderInfo : folderList) {
				if (folderInfo.fileName() == todayFolder) {
					continue;
				}
				const QDate folderDate = QDate::fromString(folderInfo.fileName(), "yyyy_MM_dd");
				if (!folderDate.isValid()) {
					continue;
				}
				if (!oldestDate.isValid() || folderDate < oldestDate) {
					oldestDate = folderDate;
					oldestFolder = folderInfo.fileName();
				}
			}

			if (oldestFolder.isEmpty()) {
				qWarning() << "[ImgSave] 磁盘空间不足且没有可删除的历史日期文件夹，停止清理";
				break;
			}

			const QString folderPath = imageSavePath + oldestFolder;
			qInfo() << "[ImgSave] 停机清理：磁盘空间不足，删除最早的日期存图文件夹:" << folderPath;
			if (!QDir(folderPath).removeRecursively()) {
				qWarning() << "[ImgSave] 删除失败（文件可能被占用），停止清理:" << folderPath;
				break;	// 删不掉则放弃，避免死循环
			}
		}

		// 清理结束后刷新磁盘空间状态，满足条件才恢复存图（状态变化时通知 UI）
		QStorageInfo storage(imageSavePath);
		if (storage.isValid() && storage.isReady()) {
			const qint64 freeBytes = storage.bytesAvailable();
			const bool enough = freeBytes >= cminFreeDiskBytes;
			const double freeGB = static_cast<double>(freeBytes) / (1024.0 * 1024.0 * 1024.0);
			_lastFreeGB.store(freeGB);
			if (_diskSpaceEnough.exchange(enough) != enough) {
				if (enough) {
					qInfo() << "[ImgSave] 停机清理后磁盘剩余空间恢复:" << QString::number(freeGB, 'f', 2)
						<< "GB，已恢复存图";
				}
				else {
					qWarning() << "[ImgSave] 停机清理后磁盘剩余空间仍不足:" << QString::number(freeGB, 'f', 2)
						<< "GB，保持暂停存图";
				}
				emit diskSpaceStateChanged(enough, freeGB);
			}
		}
		_idleCleanupRunning.store(false);
	});
}

void ImgSaveModule::start()
{
	if (imageSaveEngine)
	{
		imageSaveEngine->startEngine();
	}
}

void ImgSaveModule::stop()
{
	if (imageSaveEngine)
	{
		imageSaveEngine->stop();
	}
}
