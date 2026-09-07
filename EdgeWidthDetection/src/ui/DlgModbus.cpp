#include "DlgModbus.h"
#include "ui_DlgModbus.h"

#include <QFile>
#include <QFutureWatcher>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTextStream>
#include <QtConcurrent/qtconcurrentrun.h>
#include <functional>

#include "Modules.hpp"
#include "NumberKeyboard.h"

// 三个选项卡行号 -> 点位序号（0~24）映射，顺序与 DlgModbus.ui 布局一致
static constexpr int TAB_RW[] = { 0, 1, 2, 3, 19, 20, 21, 22, 23, 24 };	// 读写参数（数值）
static constexpr int TAB_RO[] = { 4, 5, 6, 7, 8, 9, 15, 16, 17, 18, 12 };	// 只读数据（10数值 + 系统标志）
static constexpr int TAB_BOOL[] = { 10, 11, 13, 14 };						// BOOL 控制（读写）

DlgModbus::DlgModbus(QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::DlgModbusClass())
{
	ui->setupUi(this);

	build_ui();

	build_connect();
}

DlgModbus::~DlgModbus()
{
	delete ui;
}

void DlgModbus::build_ui()
{
	initPoints();
	bindRowWidgets();
	loadPointNames();
	loadPointAddresses();

	_refreshTimer.setInterval(500);
	_refreshTimer.setSingleShot(false);
}

void DlgModbus::build_connect()
{
	connect(ui->btn_close, &QPushButton::clicked, this, &DlgModbus::btn_close_clicked);
	connect(&_refreshTimer, &QTimer::timeout, this, &DlgModbus::onRefreshTimeout);

	// 地址按钮：所有点位均可点击修改
	for (const auto& point : _points)
	{
		if (point.btnAddr)
		{
			connect(point.btnAddr, &QPushButton::clicked, this, [this, idx = point.index]()
				{
					onAddrClicked(idx);
				});
		}
	}

	// 读写参数页：写入按钮
	for (int i = 0; i < static_cast<int>(std::size(TAB_RW)); ++i)
	{
		auto* btn = findChild<QPushButton*>(QStringLiteral("btn_write_rw_%1").arg(i));
		if (btn)
		{
			connect(btn, &QPushButton::clicked, this, [this, i]()
				{
					onWriteClicked(i);
				});
		}
	}

	// BOOL 控制页：置1 / 置0 按钮
	for (int i = 0; i < static_cast<int>(std::size(TAB_BOOL)); ++i)
	{
		auto* btnSet1 = findChild<QPushButton*>(QStringLiteral("btn_set1_bool_%1").arg(i));
		auto* btnSet0 = findChild<QPushButton*>(QStringLiteral("btn_set0_bool_%1").arg(i));
		if (btnSet1)
		{
			connect(btnSet1, &QPushButton::clicked, this, [this, i]()
				{
					onBoolWriteClicked(i, true);
				});
		}
		if (btnSet0)
		{
			connect(btnSet0, &QPushButton::clicked, this, [this, i]()
				{
					onBoolWriteClicked(i, false);
				});
		}
	}
}

void DlgModbus::initPoints()
{
	struct PointDefault
	{
		const char* name;		// 默认名称（modbus.txt 缺失行时的兜底）
		int address;			// 默认地址（SetConfig 缺失时的兜底，来自《通讯地址.xlsx》）
		PointType type;
		bool writable;
	};

	// 顺序固定，与 modbus.txt 行序、SetConfig.modbusAddressList 逗号顺序一一对应
	const PointDefault defaults[] = {
		{ "R_切刀点动速度",		1000,	PointType::Float,	true  },	// 0
		{ "R_设定拍照长度",		1002,	PointType::Float,	true  },	// 1
		{ "R_自动速度",			1026,	PointType::Float,	true  },	// 2
		{ "D_间隔袋数",			1028,	PointType::Dint,	true  },	// 3
		{ "R_当前中心偏移值",	214,		PointType::Float,	false },	// 4
		{ "实际拍照值",			3000,	PointType::Float,	false },	// 5
		{ "总偏移值",			3052,	PointType::Float,	false },	// 6
		{ "编码器当前位置",		2000,	PointType::Float,	false },	// 7
		{ "编码器当前速度",		3058,	PointType::Dint,	false },	// 8
		{ "R_切刀当前位置",		2004,	PointType::Float,	false },	// 9
		{ "切刀回原",			3000,	PointType::Bool,	true  },	// 10
		{ "切刀补偿开启",		1000,	PointType::Bool,	true  },	// 11
		{ "系统标志",			3004,	PointType::Bool,	false },	// 12
		{ "启动",				3002,	PointType::Bool,	true  },	// 13
		{ "停止",				3003,	PointType::Bool,	true  },	// 14
		{ "R_白料长",			3040,	PointType::Float,	false },	// 15
		{ "R_d1袋长",			3030,	PointType::Float,	false },	// 16
		{ "切刀计算移动量",		3042,	PointType::Float,	false },	// 17
		{ "切刀实际移动量",		3060,	PointType::Float,	false },	// 18
		{ "R_编码器一圈脉冲数",	1006,	PointType::Float,	true  },	// 19
		{ "R_编码器一圈距离",	1008,	PointType::Float,	true  },	// 20
		{ "R_中心偏移最大值",	1030,	PointType::Float,	true  },	// 21
		{ "R_中心偏移最小值",	1032,	PointType::Float,	true  },	// 22
		{ "切刀移动最大值",		1034,	PointType::Float,	true  },	// 23
		{ "切刀移动最小值",		1036,	PointType::Float,	true  },	// 24
	};

	_points.clear();
	_points.reserve(static_cast<int>(std::size(defaults)));
	for (int i = 0; i < static_cast<int>(std::size(defaults)); ++i)
	{
		PointInfo info;
		info.index = i;
		info.name = QString::fromUtf8(defaults[i].name);
		info.address = defaults[i].address;
		info.type = defaults[i].type;
		info.writable = defaults[i].writable;
		_points.push_back(info);
	}
}

void DlgModbus::bindRowWidgets()
{
	auto bind = [this](const char* prefix, int row, int pointIndex)
		{
			auto& point = _points[pointIndex];
			point.lbName = findChild<QLabel*>(QStringLiteral("lb_name_%1_%2").arg(prefix).arg(row));
			point.btnAddr = findChild<QPushButton*>(QStringLiteral("btn_addr_%1_%2").arg(prefix).arg(row));
			point.lbValue = findChild<QLabel*>(QStringLiteral("lb_value_%1_%2").arg(prefix).arg(row));
		};

	for (int i = 0; i < static_cast<int>(std::size(TAB_RW)); ++i)
	{
		bind("rw", i, TAB_RW[i]);
	}
	for (int i = 0; i < static_cast<int>(std::size(TAB_RO)); ++i)
	{
		bind("ro", i, TAB_RO[i]);
	}
	for (int i = 0; i < static_cast<int>(std::size(TAB_BOOL)); ++i)
	{
		bind("bool", i, TAB_BOOL[i]);
	}
}

void DlgModbus::loadPointNames()
{
	// modbus.txt 为外部维护文件，程序只读不写；每行一个名称，逗号分割，按行序对应点位
	QFile file(globalPath.modbusTxtPath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "modbus.txt 不存在或无法打开，使用默认点位名称:" << globalPath.modbusTxtPath;
		return;
	}

	QTextStream in(&file);
	in.setEncoding(QStringConverter::Utf8);

	int lineIndex = 0;
	while (!in.atEnd() && lineIndex < _points.size())
	{
		const QString line = in.readLine();
		// 逗号分割，取第一段作为点位名称
		const QString name = line.split(',').first().trimmed();
		if (!name.isEmpty())
		{
			_points[lineIndex].name = name;
		}
		++lineIndex;
	}

	// 应用到名称标签
	for (const auto& point : _points)
	{
		if (point.lbName)
		{
			point.lbName->setText(point.name);
		}
	}
}

void DlgModbus::loadPointAddresses()
{
	const auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	const QStringList parts = QString::fromStdString(setConfig.modbusAddressList).split(',');

	for (int i = 0; i < _points.size() && i < parts.size(); ++i)
	{
		bool ok = false;
		const int addr = parts[i].toInt(&ok);
		if (ok && addr >= 0)
		{
			_points[i].address = addr;
		}
	}

	// 应用到地址按钮
	for (const auto& point : _points)
	{
		if (point.btnAddr)
		{
			point.btnAddr->setText(QString::number(point.address));
		}
	}
}

void DlgModbus::savePointAddresses()
{
	QStringList parts;
	parts.reserve(_points.size());
	for (const auto& point : _points)
	{
		parts << QString::number(point.address);
	}

	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	setConfig.modbusAddressList = parts.join(',').toStdString();

	// 使用多代备份 + 写入后验证的安全保存，防止断电导致配置文件损坏
	Modules::getInstance().configManagerModule.saveConfigSafe();
}

void DlgModbus::btn_close_clicked()
{
	this->close();
}

void DlgModbus::onRefreshTimeout()
{
	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;
	if (!plcControllerScheduler)
	{
		// 不停止定时器：PLC 重连后自动恢复刷新
		for (const auto& point : _points)
		{
			if (point.lbValue)
			{
				point.lbValue->setText(QStringLiteral("PLC未连接"));
			}
		}
		return;
	}

	if (_refreshInFlight)
	{
		return;
	}
	_refreshInFlight = true;

	// 发起全部点位的异步读取（float/DINT 小端，BOOL 读线圈），future 移到后台线程统一等待
	// std::function 要求可拷贝，future 为移动语义，故用 shared_ptr 包一层
	std::vector<std::function<QString()>> tasks;
	tasks.reserve(_points.size());
	for (const auto& point : _points)
	{
		const auto addr = static_cast<rw::hoem::Address16>(point.address);
		switch (point.type)
		{
		case PointType::Float:
		{
			auto fut = std::make_shared<std::future<std::pair<float, bool>>>(
				plcControllerScheduler->readFloatRegisterAsync(addr, rw::hoem::Endianness::LittleEndian));
			tasks.push_back([fut]() mutable
				{
					auto result = fut->get();
					return result.second ? QString::number(result.first, 'f', 2) : QStringLiteral("读取失败");
				});
			break;
		}
		case PointType::Dint:
		{
			auto fut = std::make_shared<std::future<std::pair<rw::hoem::UInt32, bool>>>(
				plcControllerScheduler->readUInt32RegisterAsync(addr, rw::hoem::Endianness::LittleEndian));
			tasks.push_back([fut]() mutable
				{
					auto result = fut->get();
					return result.second ? QString::number(static_cast<int32_t>(result.first)) : QStringLiteral("读取失败");
				});
			break;
		}
		case PointType::Bool:
		{
			auto fut = std::make_shared<std::future<std::pair<bool, bool>>>(
				plcControllerScheduler->readCoilAsync(addr));
			tasks.push_back([fut]() mutable
				{
					auto result = fut->get();
					return result.second ? QString::number(result.first ? 1 : 0) : QStringLiteral("读取失败");
				});
			break;
		}
		}
	}

	auto* watcher = new QFutureWatcher<QVector<QString>>(this);
	connect(watcher, &QFutureWatcher<QVector<QString>>::finished, this, [this, watcher]()
		{
			const auto texts = watcher->result();
			watcher->deleteLater();
			_refreshInFlight = false;
			for (int i = 0; i < texts.size() && i < _points.size(); ++i)
			{
				if (_points[i].lbValue)
				{
					_points[i].lbValue->setText(texts[i]);
				}
			}
		});
	watcher->setFuture(QtConcurrent::run([tasks = std::move(tasks)]() mutable
		{
			QVector<QString> texts;
			texts.reserve(static_cast<int>(tasks.size()));
			for (auto& task : tasks)
			{
				texts.push_back(task());
			}
			return texts;
		}));
}

void DlgModbus::onAddrClicked(int pointIndex)
{
	if (pointIndex < 0 || pointIndex >= _points.size())
	{
		return;
	}

	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toInt() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于等于0的地址");
			return;
		}

		auto& point = _points[pointIndex];
		point.address = value.toInt();
		if (point.btnAddr)
		{
			point.btnAddr->setText(value);
		}
	}
}

void DlgModbus::onWriteClicked(int rwRow)
{
	if (rwRow < 0 || rwRow >= static_cast<int>(std::size(TAB_RW)))
	{
		return;
	}
	auto& point = _points[TAB_RW[rwRow]];

	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;
	if (!plcControllerScheduler)
	{
		QMessageBox::information(this, "警告", "PLC未连接");
		return;
	}

	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept != QDialog::Accepted)
	{
		return;
	}

	const auto value = numKeyBord.getValue();
	const auto addr = static_cast<rw::hoem::Address16>(point.address);

	bool success = false;
	if (point.type == PointType::Float)
	{
		success = plcControllerScheduler->writeFloatRegisterAsync(
			addr, value.toFloat(), rw::hoem::Endianness::LittleEndian).get();
	}
	else if (point.type == PointType::Dint)
	{
		success = plcControllerScheduler->writeUInt32RegisterAsync(
			addr, static_cast<rw::hoem::UInt32>(static_cast<int32_t>(value.toInt())),
			rw::hoem::Endianness::LittleEndian).get();
	}

	if (success)
	{
		QMessageBox::information(this, "提示", point.name + " 写入成功");
		// 写入成功后立即刷新一次当前值
		onRefreshTimeout();
	}
	else
	{
		QMessageBox::warning(this, "警告", point.name + " 写入失败");
	}
}

void DlgModbus::onBoolWriteClicked(int boolRow, bool state)
{
	if (boolRow < 0 || boolRow >= static_cast<int>(std::size(TAB_BOOL)))
	{
		return;
	}
	auto& point = _points[TAB_BOOL[boolRow]];

	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;
	if (!plcControllerScheduler)
	{
		QMessageBox::information(this, "警告", "PLC未连接");
		return;
	}

	const auto addr = static_cast<rw::hoem::Address16>(point.address);
	bool success = plcControllerScheduler->writeCoilAsync(addr, state).get();

	if (success)
	{
		if (point.lbValue)
		{
			point.lbValue->setText(state ? "1" : "0");
		}
	}
	else
	{
		QMessageBox::warning(this, "警告", point.name + (state ? " 置1失败" : " 置0失败"));
	}
}

void DlgModbus::showEvent(QShowEvent* event)
{
	QDialog::showEvent(event);
	// 打开对话框时立即刷新一次，并启动定时自动刷新
	onRefreshTimeout();
	if (!_refreshTimer.isActive())
	{
		_refreshTimer.start();
	}
}

void DlgModbus::hideEvent(QHideEvent* event)
{
	// 关闭对话框后停止轮询，避免空转
	_refreshTimer.stop();
	QDialog::hideEvent(event);
}

void DlgModbus::closeEvent(QCloseEvent* event)
{
	// 关闭窗口前将界面上修改的地址持久化到 SetConfig
	savePointAddresses();
	QDialog::closeEvent(event);
}
