#include "DlgLicense.h"
#include "ui_DlgLicense.h"

#include <QClipboard>
#include <QGuiApplication>
#include <QMessageBox>

DlgLicense::DlgLicense(const QString& machineCode, QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::DlgLicenseClass())
{
	ui->setupUi(this);

	ui->lineEdit_machineCode->setText(machineCode);

	connect(ui->btn_copyMachineCode, &QPushButton::clicked, this, &DlgLicense::btn_copyMachineCode_clicked);
	connect(ui->btn_activate, &QPushButton::clicked, this, &DlgLicense::btn_activate_clicked);
	connect(ui->btn_exit, &QPushButton::clicked, this, &DlgLicense::btn_exit_clicked);
}

DlgLicense::~DlgLicense()
{
	delete ui;
}

QString DlgLicense::activationCode() const
{
	return ui->lineEdit_activationCode->text().trimmed();
}

void DlgLicense::btn_copyMachineCode_clicked()
{
	QClipboard* clipboard = QGuiApplication::clipboard();
	clipboard->setText(ui->lineEdit_machineCode->text());
	QMessageBox::information(this, "提示", "机器码已复制到剪贴板");
}

void DlgLicense::btn_activate_clicked()
{
	if (activationCode().isEmpty())
	{
		QMessageBox::warning(this, "提示", "请输入激活码");
		return;
	}
	accept();
}

void DlgLicense::btn_exit_clicked()
{
	reject();
}
