#pragma once

#include<QObject>
#include <QFuture>
#include <atomic>
#include <memory>
#include"IModule.hpp"
#include "rqw_ImageSaveEngine.h"


class ImgSaveModule
	:public QObject, public IModule<void> {
	Q_OBJECT
private:
	static constexpr int cimgSaveThreadNum = 4;
	static constexpr int cimgSaveQuality = 80;
public:
	void build() override;
	void destroy() override;
	void start() override;
	void stop() override;
public:
	std::unique_ptr<rw::rqw::ImageSaveEngine> imageSaveEngine{ nullptr };
private:
	/// 后台异步清理超过 7 天的历史图像文件夹，不阻塞启动流程
	void startCleanupOldFoldersAsync();
private:
	QFuture<void> _cleanupFuture;
	std::shared_ptr<std::atomic<bool>> _cleanupCancel{ std::make_shared<std::atomic<bool>>(false) };
};
