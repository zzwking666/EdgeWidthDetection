#pragma once

#include <QCloseEvent>
#include <QDialog>
#include <QShowEvent>
#include <QStringList>
#include <QTimer>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui { class DlgModbusClass; };
QT_END_NAMESPACE

class QLabel;
class QPushButton;

/// Modbus 通讯点位对话框：
/// - 点位完全由外部 CSV 文件定义（modbus.csv），程序启动时加载；
///   每行格式：名称,地址,类型,读写 —— 类型 float/DINT/BOOL，读写列填 读写/只读，地址允许 D/M 前缀（如 D1000、M3000）
/// - CSV 用 UTF-8 带 BOM 保存（Excel 打开不乱码）；读取时自动兼容 Excel 另存的 GBK CSV
/// - CSV 不存在时按内置默认表（与《通讯地址.xlsx》一致）生成一份，之后只改 CSV 即可维护点位
/// - 严格校验：CSV 中任何一行列数不对、地址/类型/读写列无法识别，都会弹窗列出全部错误并退出程序，
///   防止错误配置（如「只读」误写成「写入」）被静默忽略后带病运行
/// - 页签归属按 类型+读写 自动推导：BOOL+读写 → BOOL控制页；数值+读写 → 读写参数页；其余 → 只读数据页
/// - 点位地址在界面上点击修改，关闭对话框时整体写回 CSV
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

public:
	// 公开给 CSV 解析辅助函数使用
	enum class PointType { Float, Dint, Bool };

private:
	struct PointInfo
	{
		QString name;					// 显示名称
		int address{ 0 };				// 当前地址（界面可改）
		PointType type{ PointType::Float };
		bool writable{ false };

		QLabel* lbName{ nullptr };			// 名称标签
		QPushButton* btnAddr{ nullptr };	// 地址按钮（点击修改）
		QLabel* lbValue{ nullptr };			// 当前值标签（定时刷新）
		QPushButton* btnWrite{ nullptr };	// 读写参数页“写入”按钮（仅数值读写点位）
		QPushButton* btnSet1{ nullptr };	// BOOL 控制页“置1”按钮
		QPushButton* btnSet0{ nullptr };	// BOOL 控制页“置0”按钮
	};

private:
	static QVector<PointInfo> defaultPoints();	// 内置默认点位表（与《通讯地址.xlsx》一致）
	void loadPoints();		// 从 modbus.csv 加载点位；文件缺失时按默认表生成，内容非法时弹窗并退出程序
	[[noreturn]] void abortOnLoadErrors(const QStringList& errors);	// 弹窗列出 CSV 错误并退出程序
	void savePoints();		// 将当前点位（含界面上修改的地址）整体写回 modbus.csv
	void buildRows();		// 按点位 类型+读写 动态生成三个页签的行控件

private slots:
	void btn_close_clicked();
	void onRefreshTimeout();							// 定时轮询所有点位当前值
	void onAddrClicked(int pointIndex);					// 点击地址按钮，数字键盘修改地址
	void onWriteClicked(int pointIndex);				// 读写参数页“写入”按钮
	void onBoolWriteClicked(int pointIndex, bool state);	// BOOL 控制页“置1/置0”按钮

protected:
	void showEvent(QShowEvent* event) override;
	void hideEvent(QHideEvent* event) override;
	void closeEvent(QCloseEvent* event) override;

private:
	Ui::DlgModbusClass* ui;

	QVector<PointInfo> _points;			// 点位列表，由 modbus.csv 定义
	QTimer _refreshTimer;				// 当前值自动刷新定时器
	bool _refreshInFlight{ false };		// 上一次刷新未结束时跳过本次，避免堆积
};
