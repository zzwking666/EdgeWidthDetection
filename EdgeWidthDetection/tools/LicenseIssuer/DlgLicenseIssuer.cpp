#include "DlgLicenseIssuer.h"
#include "ui_DlgLicenseIssuer.h"

#include <QApplication>
#include <QClipboard>
#include <QMessageBox>

#include <actCry_ActivationInfo.hpp>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace
{
	constexpr char kProductName[] = "EdgeWidthDetection";
	constexpr char kActivationKey[] = "EdgeWidthDetectionActivationKey";

	std::chrono::system_clock::time_point parseDateTime(const std::string& str)
	{
		std::tm tmBuf{};
		std::istringstream ss(str);
		ss >> std::get_time(&tmBuf, "%Y-%m-%d");
		if (ss.fail())
		{
			throw std::runtime_error("日期格式错误，应为 YYYY-MM-DD");
		}
		tmBuf.tm_hour = 0;
		tmBuf.tm_min = 0;
		tmBuf.tm_sec = 0;
		auto timeT = std::mktime(&tmBuf);
		return std::chrono::system_clock::from_time_t(timeT);
	}

	std::string generateActivationCodeForMachine(const std::string& machineCode)
	{
		rw::actCry::ActivationInfo info;
		info.hwid = machineCode;
		info.startTime = std::chrono::system_clock::time_point{};
		info.endTime = parseDateTime("2099-12-31");
		return rw::actCry::ActivationInfo::generateActivationCode(info, kActivationKey);
	}
}

DlgLicenseIssuer::DlgLicenseIssuer(QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::DlgLicenseIssuerClass())
{
	ui->setupUi(this);

	connect(ui->btn_generate, &QPushButton::clicked, this, &DlgLicenseIssuer::onGenerate);
	connect(ui->btn_copyMachineCode, &QPushButton::clicked, this, &DlgLicenseIssuer::onCopyMachineCode);
	connect(ui->btn_copyActivationCode, &QPushButton::clicked, this, &DlgLicenseIssuer::onCopyActivationCode);
	connect(ui->btn_exit, &QPushButton::clicked, this, &QDialog::reject);
}

DlgLicenseIssuer::~DlgLicenseIssuer()
{
	delete ui;
}

void DlgLicenseIssuer::onGenerate()
{
	std::string machineCode = ui->lineEdit_machineCode->text().trimmed().toStdString();
	if (machineCode.empty())
	{
		QMessageBox::warning(this, "提示", "请先输入需要生成的机器的机器码");
		return;
	}

	try
	{
		std::string code = generateActivationCodeForMachine(machineCode);
		ui->lineEdit_activationCode->setText(QString::fromStdString(code));
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(this, "错误", QString("生成激活码失败：%1").arg(e.what()));
	}
}

void DlgLicenseIssuer::onCopyMachineCode()
{
	QString text = ui->lineEdit_machineCode->text();
	if (!text.isEmpty())
	{
		QApplication::clipboard()->setText(text);
		QMessageBox::information(this, "提示", "机器码已复制到剪贴板");
	}
}

void DlgLicenseIssuer::onCopyActivationCode()
{
	QString text = ui->lineEdit_activationCode->text();
	if (!text.isEmpty())
	{
		QApplication::clipboard()->setText(text);
		QMessageBox::information(this, "提示", "激活码已复制到剪贴板");
	}
}
