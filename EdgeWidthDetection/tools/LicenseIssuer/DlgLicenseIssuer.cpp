#include "DlgLicenseIssuer.h"
#include "ui_DlgLicenseIssuer.h"

#include <QApplication>
#include <QClipboard>
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>

#include <actCry_ActivationInfo.hpp>
#include <chrono>
#include <stdexcept>

#include "LicensePolicy.hpp"

namespace
{
	constexpr char kActivationKey[] = "EdgeWidthDetectionActivationKey";

	std::chrono::system_clock::time_point localMidnight(const QDate& date)
	{
		const QDateTime localTime(date, QTime(0, 0), Qt::LocalTime);
		return std::chrono::system_clock::from_time_t(
			static_cast<std::time_t>(localTime.toSecsSinceEpoch()));
	}

}

std::string DlgLicenseIssuer::generateActivationCodeForMachine(
	const std::string& machineCode, const QDate& expiryDate)
{
	rw::actCry::ActivationInfo info;
	info.hwid = machineCode;
	info.startTime = std::chrono::system_clock::time_point{};
	info.endTime = localMidnight(expiryDate);
	return rw::actCry::ActivationInfo::generateActivationCode(info, kActivationKey);
}

DlgLicenseIssuer::DlgLicenseIssuer(QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::DlgLicenseIssuerClass())
{
	ui->setupUi(this);

	ui->dateEdit_deadline1->setDate(QDate(2026, 11, 6));
	ui->dateEdit_deadline2->setDate(QDate(2026, 11, 7));
	ui->dateEdit_deadline3->setDate(QDate(2026, 11, 8));

	connect(ui->btn_generate, &QPushButton::clicked, this, &DlgLicenseIssuer::onGenerate);
	connect(ui->btn_exportPolicy, &QPushButton::clicked, this, &DlgLicenseIssuer::onExportPolicy);
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
	const std::string machineCode = ui->lineEdit_machineCode->text().trimmed().toStdString();
	if (machineCode.empty())
	{
		QMessageBox::warning(this, "提示", "请先输入需要生成的机器的机器码");
		return;
	}

	QDate expiryDate;
	switch (ui->comboBox_licenseLevel->currentIndex())
	{
	case 0:
		expiryDate = ui->dateEdit_deadline2->date();
		break;
	case 1:
		expiryDate = ui->dateEdit_deadline3->date();
		break;
	case 2:
		expiryDate = QDate(2099, 12, 31);
		break;
	default:
		QMessageBox::warning(this, "提示", "请选择授权级别");
		return;
	}

	try
	{
		const std::string code = generateActivationCodeForMachine(machineCode, expiryDate);
		ui->lineEdit_activationCode->setText(QString::fromStdString(code));
	}
	catch (const std::exception& e)
	{
		QMessageBox::warning(this, "错误", QString("生成激活码失败：%1").arg(e.what()));
	}
}

void DlgLicenseIssuer::onExportPolicy()
{
	const QString machineCode = ui->lineEdit_machineCode->text().trimmed();
	if (machineCode.isEmpty())
	{
		QMessageBox::warning(this, "提示", "请先输入客户机器的机器码，策略文件将与该机器绑定");
		return;
	}

	LicensePolicy::Data data;
	data.firstDeadline = ui->dateEdit_deadline1->date();
	data.secondDeadline = ui->dateEdit_deadline2->date();
	data.thirdDeadline = ui->dateEdit_deadline3->date();

	const QString path = QFileDialog::getSaveFileName(this, QStringLiteral("导出授权策略"),
		QStringLiteral("LicensePolicy.json"), QStringLiteral("JSON 文件 (*.json)"));
	if (path.isEmpty())
	{
		return;
	}

	QString error;
	if (!LicensePolicy::save(path, machineCode, data, error))
	{
		QMessageBox::warning(this, QStringLiteral("导出失败"), error);
		return;
	}
	QMessageBox::information(this, QStringLiteral("导出成功"),
		QStringLiteral("请将策略文件放到客户程序的配置目录，并保持文件名 LicensePolicy.json"));
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
