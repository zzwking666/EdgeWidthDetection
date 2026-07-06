#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class DlgLicenseIssuerClass; };
QT_END_NAMESPACE

/// LicenseIssuer 图形界面：输入机器码并生成激活码
class DlgLicenseIssuer : public QDialog
{
	Q_OBJECT

public:
	explicit DlgLicenseIssuer(QWidget* parent = nullptr);
	~DlgLicenseIssuer();

private slots:
	void onGenerate();
	void onCopyMachineCode();
	void onCopyActivationCode();

private:
	Ui::DlgLicenseIssuerClass* ui;
};
