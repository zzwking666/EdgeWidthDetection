#include "DlgModbus.h"
#include "ui_DlgModbus.h"

#include <QFile>
#include <QFutureWatcher>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSpacerItem>
#include <QStringConverter>
#include <QTextStream>
#include <QtConcurrent/qtconcurrentrun.h>
#include <cstdlib>
#include <functional>

#include "Modules.hpp"
#include "NumberKeyboard.h"

namespace
{
	// 行控件样式（与原 DlgModbus.ui 中的行样式一致，现由代码动态生成）
	const char* kAddrButtonStyle =
		"QPushButton {"
		"    padding: 6px 14px;"
		"    border: 2px solid #87CEEB;"
		"    border-radius: 4px;"
		"    background-color: white;"
		"    color: #2c3e50;"
		"    font-size: 20px;"
		"}"
		"QPushButton:hover {"
		"    border-color: #4682B4;"
		"    background-color: #F0F8FF;"
		"}"
		"QPushButton:pressed {"
		"    border-color: #2F4F4F;"
		"    background-color: #E0F0F8;"
		"}";

	const char* kValueLabelStyle =
		"QLabel {"
		"    color: #2F4F4F;"
		"    border: 1px solid #E0E0E0;"
		"    background-color: #FFFFFF;"
		"    padding: 6px 12px;"
		"    border-radius: 4px;"
		"}";

	const char* kHeaderLabelStyle =
		"QLabel {"
		"    color: #2c3e50;"
		"    font-weight: bold;"
		"}";

	// CSV 表头（首行与此一致时跳过）
	const char* kCsvHeader = "名称,地址,类型,读写";

	// 解析类型列，宽容大小写；返回 false 表示无法识别（严格校验，非法值会导致启动失败）
	bool parsePointType(const QString& text, DlgModbus::PointType& out)
	{
		const QString s = text.trimmed().toLower();
		if (s == "float" || s == "real")
		{
			out = DlgModbus::PointType::Float;
			return true;
		}
		if (s == "dint" || s == "int32" || s == "dword")
		{
			out = DlgModbus::PointType::Dint;
			return true;
		}
		if (s == "bool" || s == "bit" || s == "coil")
		{
			out = DlgModbus::PointType::Bool;
			return true;
		}
		return false;
	}

	QString pointTypeToString(DlgModbus::PointType type)
	{
		switch (type)
		{
		case DlgModbus::PointType::Dint: return QStringLiteral("DINT");
		case DlgModbus::PointType::Bool: return QStringLiteral("BOOL");
		default: return QStringLiteral("float");
		}
	}

	// 解析读写列：返回 false 表示无法识别（严格校验，如误写成「写入」会导致启动失败）
	bool parseWritable(const QString& text, bool& out)
	{
		const QString s = text.trimmed().toLower();
		if (s == QStringLiteral("读写") || s == "rw" || s == "write" || s == "w")
		{
			out = true;
			return true;
		}
		if (s == QStringLiteral("只读") || s == "ro" || s == "read" || s == "r")
		{
			out = false;
			return true;
		}
		return false;
	}

	// 解析地址列，允许 D/M 前缀（如 D1000、M3000），与《通讯地址.xlsx》写法一致
	int parseAddress(const QString& text)
	{
		QString s = text.trimmed();
		if (!s.isEmpty() && (s.startsWith('D') || s.startsWith('d') || s.startsWith('M') || s.startsWith('m')))
		{
			s.remove(0, 1);
		}
		bool ok = false;
		const int addr = s.toInt(&ok);
		return (ok && addr >= 0) ? addr : -1;
	}
}

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
	loadPoints();
	buildRows();

	_refreshTimer.setInterval(500);
	_refreshTimer.setSingleShot(false);
}

void DlgModbus::build_connect()
{
	connect(ui->btn_close, &QPushButton::clicked, this, &DlgModbus::btn_close_clicked);
	connect(&_refreshTimer, &QTimer::timeout, this, &DlgModbus::onRefreshTimeout);
	// 行内按钮在 buildRows() 生成时即完成连接
}

QVector<DlgModbus::PointInfo> DlgModbus::defaultPoints()
{
	struct PointDefault
	{
		const char* name;
		int address;
		PointType type;
		bool writable;
	};

	// 内置默认表，与《通讯地址.xlsx》一致；仅在 modbus.csv 缺失/无有效行时使用
	const PointDefault defaults[] = {
		{ "R_切刀点动速度",		1000,	PointType::Float,	true  },
		{ "R_设定拍照长度",		1002,	PointType::Float,	true  },
		{ "R_自动速度",			1026,	PointType::Float,	true  },
		{ "D_间隔袋数",			1028,	PointType::Dint,	true  },
		{ "R_当前中心偏移值",	214,	PointType::Float,	false },
		{ "实际拍照值",			3000,	PointType::Float,	false },
		{ "总偏移值",			3052,	PointType::Float,	false },
		{ "编码器当前位置",		2000,	PointType::Float,	false },
		{ "编码器当前速度",		3058,	PointType::Dint,	false },
		{ "R_切刀当前位置",		2004,	PointType::Float,	false },
		{ "切刀回原",			3000,	PointType::Bool,	true  },
		{ "切刀补偿开启",		1000,	PointType::Bool,	true  },
		{ "系统标志",			3004,	PointType::Bool,	false },
		{ "启动",				3002,	PointType::Bool,	true  },
		{ "停止",				3003,	PointType::Bool,	true  },
		{ "R_白料长",			3040,	PointType::Float,	false },
		{ "R_d1袋长",			3030,	PointType::Float,	false },
		{ "切刀计算移动量",		3042,	PointType::Float,	false },
		{ "切刀实际移动量",		3060,	PointType::Float,	false },
		{ "R_编码器一圈脉冲数",	1006,	PointType::Float,	true  },
		{ "R_编码器一圈距离",	1008,	PointType::Float,	true  },
		{ "R_中心偏移最大值",	1030,	PointType::Float,	true  },
		{ "R_中心偏移最小值",	1032,	PointType::Float,	true  },
		{ "切刀移动最大值",		1034,	PointType::Float,	true  },
		{ "切刀移动最小值",		1036,	PointType::Float,	true  },
	};

	QVector<PointInfo> points;
	points.reserve(static_cast<int>(std::size(defaults)));
	for (const auto& d : defaults)
	{
		PointInfo info;
		info.name = QString::fromUtf8(d.name);
		info.address = d.address;
		info.type = d.type;
		info.writable = d.writable;
		points.push_back(info);
	}
	return points;
}

void DlgModbus::loadPoints()
{
	// modbus.csv 为外部维护文件，每行：名称,地址,类型,读写
	// 顺序即点位顺序，可随意调整；新增/删除点位直接增删行即可
	// 严格校验：任何一行格式非法都会弹窗提示并退出程序，防止错误配置静默生效
	QFile file(globalPath.modbusCsvPath);
	if (!file.exists())
	{
		qDebug() << "modbus.csv 不存在，按内置默认表生成:" << globalPath.modbusCsvPath;
		_points = defaultPoints();
		savePoints();
		return;
	}

	if (!file.open(QIODevice::ReadOnly))
	{
		abortOnLoadErrors({ QStringLiteral("modbus.csv 无法打开（可能被占用或无权限）") });
		return;
	}

	const QByteArray raw = file.readAll();
	file.close();

	// 编码自动识别：UTF-8（带/不带 BOM），含非法 UTF-8 序列时按系统编码（GBK，兼容 Excel 另存的 CSV）
	QString text = QString::fromUtf8(raw);
	if (text.contains(QChar::ReplacementCharacter))
	{
		QStringDecoder decoder(QStringConverter::System);
		text = decoder.decode(raw);
	}
	// 去掉 BOM（trimmed() 不会去掉 U+FEFF，否则会污染首行首列）
	if (text.startsWith(QChar(0xFEFF)))
	{
		text.remove(0, 1);
	}

	_points.clear();
	QStringList errors;
	const QStringList lines = text.split('\n');
	for (int lineNo = 0; lineNo < lines.size(); ++lineNo)
	{
		const QString line = lines[lineNo].trimmed();
		if (line.isEmpty())
		{
			continue;
		}

		const QStringList fields = line.split(',');
		const QString name = fields.value(0).trimmed();
		if (name == QStringLiteral("名称"))	// 跳过表头
		{
			continue;
		}

		auto addError = [&](const QString& reason)
		{
			errors << QStringLiteral("第 %1 行「%2」：%3").arg(lineNo + 1).arg(line, reason);
		};

		if (fields.size() != 4)
		{
			addError(QStringLiteral("应为 4 列（名称,地址,类型,读写），实际为 %1 列").arg(fields.size()));
			continue;
		}
		if (name.isEmpty())
		{
			addError(QStringLiteral("名称列为空"));
			continue;
		}

		PointInfo info;
		info.name = name;
		info.address = parseAddress(fields[1]);
		if (info.address < 0)
		{
			addError(QStringLiteral("地址「%1」无效，应为非负整数（允许 D/M 前缀）").arg(fields[1].trimmed()));
			continue;
		}
		if (!parsePointType(fields[2], info.type))
		{
			addError(QStringLiteral("类型「%1」无法识别，应为 float / DINT / BOOL").arg(fields[2].trimmed()));
			continue;
		}
		if (!parseWritable(fields[3], info.writable))
		{
			addError(QStringLiteral("读写列「%1」无法识别，应为「读写」或「只读」").arg(fields[3].trimmed()));
			continue;
		}
		_points.push_back(info);
	}

	if (_points.isEmpty() && errors.isEmpty())
	{
		errors << QStringLiteral("文件中没有任何点位行");
	}

	if (!errors.isEmpty())
	{
		_points.clear();
		abortOnLoadErrors(errors);
	}
}

void DlgModbus::abortOnLoadErrors(const QStringList& errors)
{
	qWarning() << "modbus.csv 配置错误，程序退出:\n" << errors.join('\n');

	// 最多展示前 10 条，避免弹窗过长
	QStringList shown = errors.mid(0, 10);
	QString detail = shown.join('\n');
	if (errors.size() > shown.size())
	{
		detail += QStringLiteral("\n……共 %1 处错误").arg(errors.size());
	}

	QMessageBox::critical(nullptr, QStringLiteral("Modbus 配置错误"),
		QStringLiteral("%1 配置存在以下错误：\n\n%2\n\n请修正该文件后重新启动程序，程序即将退出。")
		.arg(globalPath.modbusCsvPath, detail));
	std::exit(EXIT_FAILURE);
}

void DlgModbus::savePoints()
{
	QFile file(globalPath.modbusCsvPath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
	{
		qDebug() << "modbus.csv 写入失败:" << globalPath.modbusCsvPath;
		return;
	}

	// UTF-8 带 BOM，保证 Excel 直接打开不乱码
	QTextStream out(&file);
	out.setEncoding(QStringConverter::Utf8);
	out.setGenerateByteOrderMark(true);

	out << QString::fromUtf8(kCsvHeader) << '\n';
	for (const auto& point : _points)
	{
		out << point.name << ','
			<< point.address << ','
			<< pointTypeToString(point.type) << ','
			<< (point.writable ? QStringLiteral("读写") : QStringLiteral("只读")) << '\n';
	}
}

void DlgModbus::buildRows()
{
	struct TabDef
	{
		QGridLayout* grid;
		const char* valueHeader;	// “当前值”列标题（BOOL 页为“当前状态”）
		bool hasWrite;				// 是否有“写入”按钮列（读写参数页）
		bool hasBoolOps;			// 是否有“置1/置0”按钮列（BOOL 控制页）
	};

	const TabDef tabs[] = {
		{ ui->gridLayout_rw,	"当前值",	true,	false },	// 读写参数：数值 + 读写
		{ ui->gridLayout_ro,	"当前值",	false,	false },	// 只读数据：只读数值 + 只读 BOOL
		{ ui->gridLayout_bool,	"当前状态",	false,	true  },	// BOOL 控制：BOOL + 读写
	};

	for (const auto& tab : tabs)
	{
		auto* grid = tab.grid;

		// 表头行
		const QStringList headers = tab.hasBoolOps
			? QStringList{ "点位名称", "地址(点击修改)", QString::fromUtf8(tab.valueHeader), "操作", "" }
			: (tab.hasWrite
				? QStringList{ "点位名称", "地址(点击修改)", QString::fromUtf8(tab.valueHeader), "操作" }
				: QStringList{ "点位名称", "地址(点击修改)", QString::fromUtf8(tab.valueHeader) });
		for (int col = 0; col < headers.size(); ++col)
		{
			auto* lb = new QLabel(headers[col]);
			lb->setStyleSheet(kHeaderLabelStyle);
			grid->addWidget(lb, 0, col);
		}

		// 数据行：按 类型+读写 归页，保持 _points 中的顺序
		int row = 1;
		for (int i = 0; i < _points.size(); ++i)
		{
			auto& point = _points[i];
			const bool isBoolTab = (point.type == PointType::Bool && point.writable);
			const bool isRwTab = (point.type != PointType::Bool && point.writable);
			const bool belongsHere = tab.hasBoolOps ? isBoolTab : (tab.hasWrite ? isRwTab : (!isBoolTab && !isRwTab));
			if (!belongsHere)
			{
				continue;
			}

			auto* lbName = new QLabel(point.name);
			lbName->setMinimumHeight(40);

			auto* btnAddr = new QPushButton(QString::number(point.address));
			btnAddr->setMinimumSize(120, 40);
			btnAddr->setStyleSheet(kAddrButtonStyle);

			auto* lbValue = new QLabel(QStringLiteral("--"));
			lbValue->setMinimumHeight(40);
			lbValue->setStyleSheet(kValueLabelStyle);

			point.lbName = lbName;
			point.btnAddr = btnAddr;
			point.lbValue = lbValue;

			grid->addWidget(lbName, row, 0);
			grid->addWidget(btnAddr, row, 1);
			grid->addWidget(lbValue, row, 2);

			connect(btnAddr, &QPushButton::clicked, this, [this, i]()
				{
					onAddrClicked(i);
				});

			if (tab.hasWrite)
			{
				auto* btnWrite = new QPushButton(QStringLiteral("写入"));
				btnWrite->setMinimumSize(90, 40);
				point.btnWrite = btnWrite;
				grid->addWidget(btnWrite, row, 3);
				connect(btnWrite, &QPushButton::clicked, this, [this, i]()
					{
						onWriteClicked(i);
					});
			}

			if (tab.hasBoolOps)
			{
				auto* btnSet1 = new QPushButton(QStringLiteral("置1"));
				btnSet1->setMinimumSize(90, 40);
				auto* btnSet0 = new QPushButton(QStringLiteral("置0"));
				btnSet0->setMinimumSize(90, 40);
				point.btnSet1 = btnSet1;
				point.btnSet0 = btnSet0;
				grid->addWidget(btnSet1, row, 3);
				grid->addWidget(btnSet0, row, 4);
				connect(btnSet1, &QPushButton::clicked, this, [this, i]()
					{
						onBoolWriteClicked(i, true);
					});
				connect(btnSet0, &QPushButton::clicked, this, [this, i]()
					{
						onBoolWriteClicked(i, false);
					});
			}

			++row;
		}

		// 底部弹簧，让数据行靠上排列
		grid->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding), row, 0);
		grid->setRowStretch(row, 1);
	}
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

void DlgModbus::onWriteClicked(int pointIndex)
{
	if (pointIndex < 0 || pointIndex >= _points.size())
	{
		return;
	}
	auto& point = _points[pointIndex];

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

void DlgModbus::onBoolWriteClicked(int pointIndex, bool state)
{
	if (pointIndex < 0 || pointIndex >= _points.size())
	{
		return;
	}
	auto& point = _points[pointIndex];

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
	// 关闭窗口前将界面上修改的地址随点位表整体写回 modbus.csv
	savePoints();
	QDialog::closeEvent(event);
}
