#include <QMessageBox>
#include <QtWidgets/QApplication>
#include <windows.h>

#include "Modules.hpp"
#include "EdgeWidthDetection.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);

	if (!Modules::check())
	{
		return 1;
	}

	Modules::getInstance().build();

	EdgeWidthDetection w;
	Modules::getInstance().uiModule._maiLiDingZi = &w;
	Modules::getInstance().connect();
	Modules::getInstance().start();

	w.setFixedSize(1920, 1080);
#ifdef NDEBUG
	w.showFullScreen();
#else
	//w.show();
	w.showFullScreen();
#endif
    return a.exec();
}
