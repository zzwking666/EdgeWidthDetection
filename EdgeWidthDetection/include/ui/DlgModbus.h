#pragma once

#include <QCloseEvent>
#include <QDialog>
#include <QShowEvent>
#include <QTimer>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui { class DlgModbusClass; };
QT_END_NAMESPACE

class QLabel;
class QPushButton;

/// Modbus 通讯点位对话框：
/// - 点位共 25 个，顺序固定（与《通讯地址.xlsx》一致）
/// - 点位名称来自外部配置文件 modbus.txt（程序只读，仅允许外界手动修改，按行序逗号分割）
/// - 点位地址在界面上点击修改，关闭对话框时持久化到 SetConfig.modbusAddressList
/// - 打开对话框后定时自动刷新所有点位当前值（float/DINT 小端，BOOL 为线圈）
class DlgModbus : public QDialog
{
	Q_OBJECT

public:
	DlgModbus(QWidget* parent = nullptr);
	~DlgModbus();

public:
	void build_ui();
	void build_connect();

private:
	enum class PointType { Float, Dint, Bool };

	struct PointInfo
	{
		int index{ 0 };					// 点位序号（0~24）
		QString name;					// 显示名称（modbus.txt 按行序覆盖默认值）
		int address{ 0 };				// 当前地址（界面可改）
		PointType type{ PointType::Float };
		bool writable{ false };

		QLabel* lbName{ nullptr };		// 名称标签
		QPushButton* btnAddr{ nullptr };// 地址按钮（点击修改）
		QLabel* lbValue{ nullptr };		// 当前值标签（定时刷新）
	};

private:
	void initPoints();			// 初始化 25 个固定点位（默认名称/地址/类型）
	void bindRowWidgets();		// 按 objectName 查找各行控件并缓存到 _points
	void loadPointNames();		// 从 modbus.txt 读取点位名称（只读，不写入）
	void loadPointAddresses();	// 从 SetConfig.modbusAddressList 读取地址
	void savePointAddresses();	// 将当前地址写回 SetConfig 并安全保存到本地

private slots:
	void btn_close_clicked();
	void onRefreshTimeout();					// 定时轮询所有点位当前值
	void onAddrClicked(int pointIndex);			// 点击地址按钮，数字键盘修改地址
	void onWriteClicked(int rwRow);				// 读写参数页“写入”按钮
	void onBoolWriteClicked(int boolRow, bool state);	// BOOL 控制页“置1/置0”按钮

protected:
	void showEvent(QShowEvent* event) override;
	void hideEvent(QHideEvent* event) override;
	void closeEvent(QCloseEvent* event) override;

private:
	Ui::DlgModbusClass* ui;

	QVector<PointInfo> _points;			// 25 个点位，顺序固定
	QTimer _refreshTimer;				// 当前值自动刷新定时器
	bool _refreshInFlight{ false };		// 上一次刷新未结束时跳过本次，避免堆积
};
