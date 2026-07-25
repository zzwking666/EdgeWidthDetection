#include "ImgSaveModule.hpp"

#include <QDate>
#include <QDir>
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
}

void ImgSaveModule::destroy()
{
	// 取消后台清理并等待其退出（取消以单个文件夹为粒度生效，正在删除的文件夹会删完才退出）
	_cleanupCancel->store(true);
	if (_cleanupFuture.isStarted() && !_cleanupFuture.isFinished()) {
		_cleanupFuture.waitForFinished();
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
