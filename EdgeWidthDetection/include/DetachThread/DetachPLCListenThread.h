#pragma once

#include <QThread>
#include <atomic>
#include"rqw_LabelWarning.h"

class DetachPLCListenThread : public QThread
{
	Q_OBJECT
public:
	explicit DetachPLCListenThread(QObject* parent = nullptr);

	~DetachPLCListenThread() override;

	void startThread();

	void stopThread();
protected:
	void run() override;
signals:
	void updatePLCInfo(uint16_t result);
private:
	void readPLCInfo();
private:
	std::atomic<bool> running; // 使用原子变量保证线程安全
};
