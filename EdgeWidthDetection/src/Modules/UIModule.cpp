#include "UIModule.hpp"
#include "DlgModbus.h"
#include "DlgProductSet.h"
#include "EdgeWidthDetection.h"
#include "ui_DlgProductSet.h"

void UIModule::build()
{
	_dlgProductSet = new DlgProductSet();
	_dlgModbus = new DlgModbus();
}

void UIModule::destroy()
{
	if (_dlgProductSet)
	{
		delete _dlgProductSet;
		_dlgProductSet = nullptr;
	}
	if (_dlgModbus)
	{
		delete _dlgModbus;
		_dlgModbus = nullptr;
	}
}

void UIModule::start()
{
}

void UIModule::stop()
{
}