#include "DlgCloseForm.h"

#include <QProcess>
#include <QApplication>
// ---- Windows 平台头文件整理修复 C3646 nLength 问题 ----
#ifdef Q_OS_WIN
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// 只包含 <Windows.h>，避免直接单独包含 winbase.h / winuser.h 造成类型前置声明缺失
#include <Windows.h>
#endif
// ------------------------------------------------------

#include "ui_DlgCloseForm.h"
DlgCloseForm::DlgCloseForm(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::DlgCloseFormClass())
{
	ui->setupUi(this);
    build_connect();
}

DlgCloseForm::~DlgCloseForm()
{
	delete ui;
}

void DlgCloseForm::build_connect()
{
    connect(ui->btn_close, &QPushButton::clicked, this, &DlgCloseForm::btn_close_clicked);
    connect(ui->btn_restart, &QPushButton::clicked, this, &DlgCloseForm::btn_restart_clicked);
    connect(ui->btn_yes, &QPushButton::clicked, this, &DlgCloseForm::btn_yes_clicked);
	connect(ui->btn_no, &QPushButton::clicked, this, &DlgCloseForm::btn_no_clicked);
}

void DlgCloseForm::setLabelShowMessage(QString message)
{
    if (ui && ui->label) {
        ui->label->setText(message);
    }
}

void DlgCloseForm::btn_close_clicked()
{
	this->close();
}

void DlgCloseForm::btn_restart_clicked()
{
    // 先退出 Qt 应用，让主窗口析构完成配置保存，再由系统延迟重启，
    // 避免系统重启强杀进程导致内存中的配置丢失
    qApp->quit();

#ifdef Q_OS_WIN
    HANDLE hToken = nullptr;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        TOKEN_PRIVILEGES tkp{};
        if (LookupPrivilegeValue(nullptr, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid)) {
            tkp.PrivilegeCount = 1;
            tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
            AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, nullptr, nullptr);
        }
        CloseHandle(hToken);
    }

    // 延迟 5 秒重启，给应用退出和配置落盘留出时间；detached 进程不受本应用退出影响
    if (!QProcess::startDetached("shutdown", { "/r", "/f", "/t", "5" })) {
        // 回退：立即重启（存在应用未退出即被强杀的风险）
        if (!ExitWindowsEx(EWX_REBOOT | EWX_FORCEIFHUNG,
            SHTDN_REASON_MAJOR_APPLICATION | SHTDN_REASON_MINOR_OTHER | SHTDN_REASON_FLAG_PLANNED)) {
            QProcess::startDetached("shutdown", { "/r", "/t", "7" });
        }
    }
#else
    if (!QProcess::startDetached("systemctl", { "reboot" })) {
        if (!QProcess::startDetached("reboot")) {
            QProcess::startDetached("shutdown", { "-r", "+1" }); // 1 分钟缓冲
        }
    }
#endif
}

void DlgCloseForm::btn_yes_clicked()
{
    // 先退出 Qt 应用，让主窗口析构完成配置保存，再由系统延迟关机，
    // 避免关机强杀进程导致内存中的配置丢失
    qApp->quit();

#ifdef Q_OS_WIN
    // 提升关机权限（与重启相同）
    HANDLE hToken = nullptr;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        TOKEN_PRIVILEGES tkp{};
        if (LookupPrivilegeValue(nullptr, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid)) {
            tkp.PrivilegeCount = 1;
            tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
            AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, nullptr, nullptr);
        }
        CloseHandle(hToken);
    }

    // 延迟 5 秒关机并切断电源，给应用退出和配置落盘留出时间；detached 进程不受本应用退出影响
    if (!QProcess::startDetached("shutdown", { "/s", "/f", "/t", "5" })) {
        // 回退：立即关机（存在应用未退出即被强杀的风险）
        if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_POWEROFF | EWX_FORCEIFHUNG,
            SHTDN_REASON_MAJOR_APPLICATION | SHTDN_REASON_MINOR_OTHER | SHTDN_REASON_FLAG_PLANNED)) {
            QProcess::startDetached("shutdown", { "/s", "/t", "7" });
        }
    }
#else
    // 依次尝试 systemd、传统命令、再回退到延迟方式
    if (!QProcess::startDetached("systemctl", { "poweroff" })) {
        if (!QProcess::startDetached("poweroff")) {
            QProcess::startDetached("shutdown", { "-h", "+1" }); // 1 分钟缓冲
        }
    }
#endif
}

void DlgCloseForm::btn_no_clicked()
{
    qApp->quit();
}

