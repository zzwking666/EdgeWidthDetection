#pragma once

#include <QDialog>
#include <QDate>
#include <string>

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

	/// 同时供图形界面和命令行测试/签发入口使用
	static std::string generateActivationCodeForMachine(const std::string& machineCode, const QDate& expiryDate);

private slots:
	void onGenerate();
	void onExportPolicy();
	void onCopyMachineCode();
	void onCopyActivationCode();

private:
	Ui::DlgLicenseIssuerClass* ui;
};
