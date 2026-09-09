#include "DlgModbus.h"
#include "ui_DlgModbus.h"

#include <QFile>
#include <QCheckBox>
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
	const char* kCsvHeader = "页签,名称,地址,类型,协议地址,读写";

	// 六个固定页签名，与《通讯地址.xlsx》的六个 sheet 一一对应，顺序即页签顺序
	const QStringList kTabNames = {
		QStringLiteral("D寄存器读写"),
		QStringLiteral("D寄存器只读"),
		QStringLiteral("M寄存器读写"),
		QStringLiteral("M寄存器只读"),
		QStringLiteral("X寄存器只读"),
		QStringLiteral("Y寄存器读写"),
	};

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

	// 校验寄存器地址列（显示用，如 D1000、M3000、X7）：寄存器字母前缀 + 数字
	bool isValidRegisterAddress(const QString& text)
	{
		const QString s = text.trimmed();
		if (s.size() < 2)
		{
			return false;
		}
		const QChar prefix = s.at(0).toUpper();
		if (prefix != 'D' && prefix != 'M' && prefix != 'X' && prefix != 'Y')
		{
			return false;
		}
		for (int i = 1; i < s.size(); ++i)
		{
			if (!s.at(i).isDigit())
			{
				return false;
			}
		}
		return true;
	}

	// 解析协议地址列（实际写入 Modbus 的地址），必须是非负整数
	int parseProtocolAddress(const QString& text)
	{
		bool ok = false;
		const int addr = text.trimmed().toInt(&ok);
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
	// 「允许写入」总开关：状态写入 PlcController，主界面面板共用；同时启停本对话框各行写入按钮
	connect(ui->ckb_enableWrite, &QCheckBox::toggled, this, [this](bool checked)
		{
			Modules::getInstance().plcController.manualWriteEnabled.store(checked);
			applyWriteEnabled(checked);
		});
	// 行内按钮在 buildRows() 生成时即完成连接
}

void DlgModbus::applyWriteEnabled(bool enabled)
{
	for (const auto& point : _points)
	{
		if (point.btnWrite)
		{
			point.btnWrite->setEnabled(enabled);
		}
		if (point.btnSet1)
		{
			point.btnSet1->setEnabled(enabled);
		}
		if (point.btnSet0)
		{
			point.btnSet0->setEnabled(enabled);
		}
	}
}

QVector<DlgModbus::PointInfo> DlgModbus::defaultPoints()
{
	struct PointDefault
	{
		const char* tab;
		const char* name;
		const char* address;
		PointType type;
		int protocolAddress;
		bool writable;
	};

	// 内置默认表，与新版《通讯地址.xlsx》六个 sheet 一致；仅在 modbus.csv 缺失时使用
	const PointDefault defaults[] = {
		{ "D寄存器读写",	"R_切刀一直点动速度",	"D1000",	PointType::Float,	1000,	true  },
		{ "D寄存器读写",	"R_设定拍照长度",		"D1002",	PointType::Float,	1002,	true  },
		{ "D寄存器读写",	"R_自动速度",			"D1026",	PointType::Float,	1026,	true  },
		{ "D寄存器读写",	"D_间隔袋数",			"D1028",	PointType::Dint,	1028,	true  },
		{ "D寄存器读写",	"R_编码器一圈脉冲数",	"D1006",	PointType::Float,	1006,	true  },
		{ "D寄存器读写",	"R_编码器一圈距离",		"D1008",	PointType::Float,	1008,	true  },
		{ "D寄存器读写",	"R_中心偏移最大值",		"D1030",	PointType::Float,	1030,	true  },
		{ "D寄存器读写",	"R_中心偏移最小值",		"D1032",	PointType::Float,	1032,	true  },
		{ "D寄存器读写",	"切刀移动最大值",		"D1034",	PointType::Float,	1034,	true  },
		{ "D寄存器读写",	"切刀移动最小值",		"D1036",	PointType::Float,	1036,	true  },
		{ "D寄存器读写",	"R_切刀单次点动速度",	"D1038",	PointType::Float,	1038,	true  },
		{ "D寄存器只读",	"R_当前中心偏移值",		"D214",		PointType::Float,	214,	false },
		{ "D寄存器只读",	"实际拍照值",			"D3000",	PointType::Float,	3000,	false },
		{ "D寄存器只读",	"总偏移值",				"D3052",	PointType::Float,	3052,	false },
		{ "D寄存器只读",	"编码器当前位置",		"D2000",	PointType::Float,	2000,	false },
		{ "D寄存器只读",	"编码器当前速度",		"D3058",	PointType::Dint,	3058,	false },
		{ "D寄存器只读",	"R_切刀当前位置",		"D2004",	PointType::Float,	2004,	false },
		{ "D寄存器只读",	"R_白料长",				"D3040",	PointType::Float,	3040,	false },
		{ "D寄存器只读",	"R_d1袋长",				"D3030",	PointType::Float,	3030,	false },
		{ "D寄存器只读",	"切刀计算移动量",		"D3042",	PointType::Float,	3042,	false },
		{ "D寄存器只读",	"切刀实际移动量",		"D3060",	PointType::Float,	3060,	false },
		{ "M寄存器读写",	"切刀回原",				"M3000",	PointType::Bool,	12952,	true  },
		{ "M寄存器读写",	"切刀补偿开启",			"M1000",	PointType::Bool,	3000,	true  },
		{ "M寄存器读写",	"启动",					"M3002",	PointType::Bool,	12954,	true  },
		{ "M寄存器读写",	"停止",					"M3003",	PointType::Bool,	12955,	true  },
		{ "M寄存器只读",	"系统标志",				"M3004",	PointType::Bool,	12956,	false },
		{ "X寄存器只读",	"切刀正限位",			"X7",		PointType::Bool,	1207,	false },
		{ "X寄存器只读",	"切刀负限位",			"X4",		PointType::Bool,	1204,	false },
		{ "Y寄存器读写",	"压痕拍照输出",			"Y3",		PointType::Bool,	3,		true  },
		{ "Y寄存器读写",	"切刀拍照输出",			"Y4",		PointType::Bool,	4,		true  },
	};

	QVector<PointInfo> points;
	points.reserve(static_cast<int>(std::size(defaults)));
	for (const auto& d : defaults)
	{
		PointInfo info;
		info.tab = QString::fromUtf8(d.tab);
		info.name = QString::fromUtf8(d.name);
		info.address = QString::fromUtf8(d.address);
		info.type = d.type;
		info.protocolAddress = d.protocolAddress;
		info.writable = d.writable;
		points.push_back(info);
	}
	return points;
}

void DlgModbus::loadPoints()
{
	// modbus.csv 为外部维护文件，每行：页签,名称,地址,类型,协议地址,读写
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
		if (fields.value(0).trimmed() == QStringLiteral("页签"))	// 跳过表头
		{
			continue;
		}

		auto addError = [&](const QString& reason)
		{
			errors << QStringLiteral("第 %1 行「%2」：%3").arg(lineNo + 1).arg(line, reason);
		};

		if (fields.size() != 6)
		{
			addError(QStringLiteral("应为 6 列（页签,名称,地址,类型,协议地址,读写），实际为 %1 列").arg(fields.size()));
			continue;
		}

		PointInfo info;
		info.tab = fields[0].trimmed();
		if (!kTabNames.contains(info.tab))
		{
			addError(QStringLiteral("页签「%1」无法识别，应为六个固定页签之一：%2")
				.arg(info.tab, kTabNames.join(QStringLiteral("、"))));
			continue;
		}

		info.name = fields[1].trimmed();
		if (info.name.isEmpty())
		{
			addError(QStringLiteral("名称列为空"));
			continue;
		}

		info.address = fields[2].trimmed();
		if (!isValidRegisterAddress(info.address))
		{
			addError(QStringLiteral("地址「%1」无效，应为寄存器地址写法（如 D1000、M3000、X7、Y3）").arg(info.address));
			continue;
		}

		if (!parsePointType(fields[3], info.type))
		{
			addError(QStringLiteral("类型「%1」无法识别，应为 float / DINT / BOOL").arg(fields[3].trimmed()));
			continue;
		}

		info.protocolAddress = parseProtocolAddress(fields[4]);
		if (info.protocolAddress < 0)
		{
			addError(QStringLiteral("协议地址「%1」无效，应为非负整数").arg(fields[4].trimmed()));
			continue;
		}

		if (!parseWritable(fields[5], info.writable))
		{
			addError(QStringLiteral("读写列「%1」无法识别，应为「读写」或「只读」").arg(fields[5].trimmed()));
			continue;
		}

		// 读写列必须与页签名一致（页签含「只读」则读写列应为只读，反之亦然），防止两列互相矛盾
		const bool tabWritable = !info.tab.contains(QStringLiteral("只读"));
		if (tabWritable != info.writable)
		{
			addError(QStringLiteral("读写列「%1」与页签「%2」矛盾").arg(fields[5].trimmed(), info.tab));
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
		out << point.tab << ','
			<< point.name << ','
			<< point.address << ','
			<< pointTypeToString(point.type) << ','
			<< point.protocolAddress << ','
			<< (point.writable ? QStringLiteral("读写") : QStringLiteral("只读")) << '\n';
	}
}

void DlgModbus::buildRows()
{
	struct TabDef
	{
		QString name;		// 页签名（与 CSV 页签列对应）
		QGridLayout* grid;
	};

	const TabDef tabs[] = {
		{ kTabNames[0], ui->gridLayout_dRw },
		{ kTabNames[1], ui->gridLayout_dRo },
		{ kTabNames[2], ui->gridLayout_mRw },
		{ kTabNames[3], ui->gridLayout_mRo },
		{ kTabNames[4], ui->gridLayout_xRo },
		{ kTabNames[5], ui->gridLayout_yRw },
	};

	for (const auto& tab : tabs)
	{
		auto* grid = tab.grid;

		// 表头行：点位名称 / 地址 / 当前值，读写页签多一列“操作”
		const bool tabWritable = !tab.name.contains(QStringLiteral("只读"));
		QStringList headers{ QStringLiteral("点位名称"), QStringLiteral("地址"), QStringLiteral("当前值") };
		if (tabWritable)
		{
			headers << QStringLiteral("操作");
		}
		for (int col = 0; col < headers.size(); ++col)
		{
			auto* lb = new QLabel(headers[col]);
			lb->setStyleSheet(kHeaderLabelStyle);
			grid->addWidget(lb, 0, col);
		}

		// 数据行：按 CSV 页签列归页，保持 _points 中的顺序
		int row = 1;
		for (int i = 0; i < _points.size(); ++i)
		{
			auto& point = _points[i];
			if (point.tab != tab.name)
			{
				continue;
			}

			auto* lbName = new QLabel(point.name);
			lbName->setMinimumHeight(40);

			auto* lbAddress = new QLabel(point.address);
			lbAddress->setMinimumHeight(40);
			lbAddress->setStyleSheet(kValueLabelStyle);

			auto* lbValue = new QLabel(QStringLiteral("--"));
			lbValue->setMinimumHeight(40);
			lbValue->setStyleSheet(kValueLabelStyle);

			point.lbName = lbName;
			point.lbAddress = lbAddress;
			point.lbValue = lbValue;

			grid->addWidget(lbName, row, 0);
			grid->addWidget(lbAddress, row, 1);
			grid->addWidget(lbValue, row, 2);

			// 读写页签提供操作按钮：数值点位为“写入”，BOOL 点位为“置1/置0”
			if (point.writable && point.type != PointType::Bool)
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
			else if (point.writable && point.type == PointType::Bool)
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
		const auto addr = static_cast<rw::hoem::Address16>(point.protocolAddress);
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

void DlgModbus::onWriteClicked(int pointIndex)
{
	if (pointIndex < 0 || pointIndex >= _points.size())
	{
		return;
	}
	auto& point = _points[pointIndex];

	if (!Modules::getInstance().plcController.manualWriteEnabled.load())
	{
		QMessageBox::information(this, "提示", "Modbus 手动写入已被禁用，如需写入请先勾选右上角「允许写入」");
		return;
	}

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
	const auto addr = static_cast<rw::hoem::Address16>(point.protocolAddress);

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

	if (!Modules::getInstance().plcController.manualWriteEnabled.load())
	{
		QMessageBox::information(this, "提示", "Modbus 手动写入已被禁用，如需写入请先勾选右上角「允许写入」");
		return;
	}

	auto& plcControllerScheduler = Modules::getInstance().plcController.plcControllerScheduler;
	if (!plcControllerScheduler)
	{
		QMessageBox::information(this, "警告", "PLC未连接");
		return;
	}

	const auto addr = static_cast<rw::hoem::Address16>(point.protocolAddress);
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
	// 同步「允许写入」总开关显示状态（setChecked 会触发 toggled，顺带刷新各行写入按钮可用性）
	ui->ckb_enableWrite->setChecked(Modules::getInstance().plcController.manualWriteEnabled.load());
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
