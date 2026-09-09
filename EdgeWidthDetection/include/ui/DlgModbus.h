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
///   每行格式：页签,名称,地址,类型,协议地址,读写 —— 页签为六个固定页之一（D寄存器读写/D寄存器只读/M寄存器读写/
///   M寄存器只读/X寄存器只读/Y寄存器读写，与《通讯地址.xlsx》的六个 sheet 一一对应）；
///   类型 float/DINT/BOOL；地址列为寄存器地址（如 D1000、M3000、X7，仅显示用）；
///   协议地址列为实际写入 Modbus 的地址（如 M3000 对应 12952）；读写列填 读写/只读
/// - CSV 用 UTF-8 带 BOM 保存（Excel 打开不乱码）；读取时自动兼容 Excel 另存的 GBK CSV
/// - CSV 不存在时按内置默认表（与《通讯地址.xlsx》一致）生成一份，之后只改 CSV 即可维护点位
/// - 严格校验：CSV 中任何一行列数不对、页签/地址/类型/协议地址/读写列无法识别或读写列与页签矛盾，
///   都会弹窗列出全部错误并退出程序，防止错误配置被静默忽略后带病运行
/// - 页签归属由「页签」列直接决定，与六个 sheet 一一对应
/// - 界面上不提供地址修改功能，一切以 CSV 文件为准；读写页签提供“写入/置1/置0”操作按钮
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

	// 按点位名称查询可写 BOOL 点位（线圈）的协议地址；未找到或该点位不是可写 BOOL 时返回 false。
	// 供主界面测试按钮等按 CSV 点位名写线圈的场景使用，地址以 modbus.csv 为准
	bool findCoilProtocolAddress(const QString& name, int& outProtocolAddress) const;

public:
	// 公开给 CSV 解析辅助函数使用
	enum class PointType { Float, Dint, Bool };

private:
	struct PointInfo
	{
		QString tab;					// 所属页签名（六个固定页签之一）
		QString name;					// 显示名称
		QString address;				// 寄存器地址（如 D1000、X7，仅显示用）
		PointType type{ PointType::Float };
		int protocolAddress{ 0 };		// Modbus 协议地址（实际读写报文使用）
		bool writable{ false };

		QLabel* lbName{ nullptr };			// 名称标签
		QLabel* lbAddress{ nullptr };		// 地址标签（只读显示）
		QLabel* lbValue{ nullptr };			// 当前值标签（定时刷新）
		QPushButton* btnWrite{ nullptr };	// 读写页“写入”按钮（仅数值读写点位）
		QPushButton* btnSet1{ nullptr };	// 读写页“置1”按钮（仅 BOOL 读写点位）
		QPushButton* btnSet0{ nullptr };	// 读写页“置0”按钮（仅 BOOL 读写点位）
	};

private:
	static QVector<PointInfo> defaultPoints();	// 内置默认点位表（与《通讯地址.xlsx》一致）
	void loadPoints();		// 从 modbus.csv 加载点位；文件缺失时按默认表生成，内容非法时弹窗并退出程序
	[[noreturn]] void abortOnLoadErrors(const QStringList& errors);	// 弹窗列出 CSV 错误并退出程序
	void savePoints();		// 将点位表整体写入 modbus.csv（仅文件缺失生成默认表时调用）
	void buildRows();		// 按点位的页签列动态生成六个页签的行控件

private slots:
	void btn_close_clicked();
	void onRefreshTimeout();							// 定时轮询所有点位当前值
	void onWriteClicked(int pointIndex);				// 读写页数值点位“写入”按钮
	void onBoolWriteClicked(int pointIndex, bool state);	// 读写页 BOOL 点位“置1/置0”按钮

protected:
	void showEvent(QShowEvent* event) override;
	void hideEvent(QHideEvent* event) override;

private:
	Ui::DlgModbusClass* ui;

	QVector<PointInfo> _points;			// 点位列表，由 modbus.csv 定义
	QTimer _refreshTimer;				// 当前值自动刷新定时器
	bool _refreshInFlight{ false };		// 上一次刷新未结束时跳过本次，避免堆积
};
