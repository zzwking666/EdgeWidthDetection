#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class DlgLicenseClass; };
QT_END_NAMESPACE

/// 授权激活对话框：显示机器码并接收用户输入的激活码
class DlgLicense : public QDialog
{
	Q_OBJECT

public:
	explicit DlgLicense(const QString& machineCode, QWidget* parent = nullptr);
	~DlgLicense();

public:
	/// 获取用户输入的激活码
	QString activationCode() const;

private slots:
	void btn_copyMachineCode_clicked();
	void btn_activate_clicked();
	void btn_exit_clicked();

private:
	Ui::DlgLicenseClass* ui;
};
