#include <QMessageBox>
#include <QSessionManager>
#include <QtWidgets/QApplication>
#include <windows.h>

#include "Modules.hpp"
#include "EdgeWidthDetection.h"
#include "imgPro_Logger.hpp"
#include "lgm_Logger.hpp"
#include "lgm_PreDef.hpp"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);

	rw::lgm::LoggerCfg logger;
	rw::lgm::iniLogger(logger);

	if (!Modules::check())
	{
		return 1;
	}

	Modules::getInstance().build();

	EdgeWidthDetection w;
	Modules::getInstance().uiModule._edgeWidthDetection = &w;
	Modules::getInstance().connect();
	Modules::getInstance().start();

	// 系统关机/注销时（如 UPS 触发的自动关机），Windows 会先发 WM_QUERYENDSESSION，
	// Qt 将其转为 commitDataRequest 信号。在此处立即保存配置，不等主窗口析构，
	// 防止关机流程被强制终止时（shutdown /f 或超时强杀）来不及走 destroy()。
	// 注意：此阶段 Windows 只给每个进程几秒时间，禁止弹窗或耗时操作。
	QObject::connect(&a, &QGuiApplication::commitDataRequest, &a,
		[](QSessionManager& sessionManager) {
			Q_UNUSED(sessionManager);
			auto& configManager = Modules::getInstance().configManagerModule;
			bool saveOk = configManager.saveEdgeWidthDetectionConfigSafe()
				&& configManager.saveConfigSafe();
			Modules::writeUpsRecord(QString("系统关机（UPS 触发或手动），配置保存%1")
				.arg(saveOk ? "成功" : "失败"));
			LOG_INFO("检测到系统关机，配置保存{}", saveOk ? "成功" : "失败");
			LOG_FLUSH();
		});

	w.setFixedSize(1920, 1080);
#ifdef NDEBUG
	w.showFullScreen();
#else
	//w.show();
	w.showFullScreen();
#endif
    return a.exec();
}
