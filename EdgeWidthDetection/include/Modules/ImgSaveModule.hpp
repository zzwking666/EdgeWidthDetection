#pragma once

#include<QObject>
#include <QFuture>
#include <atomic>
#include <memory>
#include"IModule.hpp"
#include "rqw_ImageSaveEngine.h"

class QTimer;

class ImgSaveModule
	:public QObject, public IModule<void> {
	Q_OBJECT
private:
	static constexpr int cimgSaveThreadNum = 2;
	static constexpr int cimgSaveQuality = 80;
	// 存图磁盘剩余空间阈值：低于 10GB 时暂停存图
	static constexpr qint64 cminFreeDiskBytes = 10ll * 1024 * 1024 * 1024;
	// 磁盘空间检测周期：10 分钟
	static constexpr int cdiskCheckIntervalMs = 10 * 60 * 1000;
public:
	void build() override;
	void destroy() override;
	void start() override;
	void stop() override;
public:
	std::unique_ptr<rw::rqw::ImageSaveEngine> imageSaveEngine{ nullptr };
public:
	/// 存图磁盘剩余空间是否充足（由后台定时检测更新），存图前必须先检查此标志位
	bool isDiskSpaceEnough() const { return _diskSpaceEnough.load(); }
private:
	/// 后台异步清理超过 7 天的历史图像文件夹，不阻塞启动流程
	void startCleanupOldFoldersAsync();
	/// 检查存图磁盘剩余空间，低于阈值则将 _diskSpaceEnough 置为 false
	void checkDiskSpace();
private:
	QFuture<void> _cleanupFuture;
	std::shared_ptr<std::atomic<bool>> _cleanupCancel{ std::make_shared<std::atomic<bool>>(false) };
	// 磁盘空间是否充足的原子标志位，供存图线程无锁读取
	std::atomic<bool> _diskSpaceEnough{ true };
	QTimer* _diskCheckTimer{ nullptr };
};
