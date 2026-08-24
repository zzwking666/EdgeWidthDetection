#pragma once
#include <QThread>
#include <QDebug>
#include <atomic>

class CameraAndCardStateThread : public QThread
{
	Q_OBJECT
private:
	static size_t runtimeCounts;

public:
	explicit CameraAndCardStateThread(QObject* parent = nullptr);

	~CameraAndCardStateThread() override;

	void startThread();

	void stopThread();
protected:
	void run() override;
private:
	void check_cameraState();
	void check_cameraState1();
	void check_cameraState2();

	// PLC 断连检测：流程与相机一致，断连时销毁、空指针时重建
	void check_plcState();
	// 实际读一个寄存器探测连接是否存活（本地"已连接"标志在 TCP 半开时不可信）
	bool probe_plcConnected();
signals:
	void updateCameraLabelState(int cameraIndex, bool state);
signals:
	void buildCamera(int index);

	void destroyCamera(int index);

	void startMonitor(int index);

	void buildPlc();

	void destroyPlc();
private:
	std::atomic<bool> running; // 使用原子变量保证线程安全
};