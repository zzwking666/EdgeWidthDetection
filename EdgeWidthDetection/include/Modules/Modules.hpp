#pragma once

#include"IModule.hpp"

#include"AutoExposureModule.hpp"
#include"CameraModule.hpp"
#include"ConfigManagerModule.hpp"
#include "EliminateModule.hpp"
#include "ImgProModule.hpp"
#include "ImgSaveModule.hpp"
#include "oso_func.hpp"
#include "RuntimeInfoModule.hpp"
#include "UIModule.hpp"
#include "UpsMonitorModule.hpp"
#include "ReconnectModule.hpp"
#include "rqw_RunEnvCheck.hpp"
#include "TestModule.hpp"
#include "WarningModule.hpp"
#include "PlcController.hpp"

class Modules
	:public IModule<bool>
{

#pragma region Construction
public:
	static Modules& getInstance()
	{
		static Modules instance;
		return instance;
	}

	Modules(const Modules&) = delete;
	Modules& operator=(const Modules&) = delete;

private:
	Modules();
	~Modules() override;

public:
	bool build() override;
	void destroy() override;
	void start() override;
	void stop() override;
	void connect();
#pragma endregion
public:
	static bool check();
	/// 向 upsRecord 目录写一条断电/关机事件凭证文件，内容含时间戳，用于现场验证配置已保存
	static void writeUpsRecord(const QString& content);
	template<class TypeCanToAssembly>
	static void checkFileExistAndFormat(const QString& path, const rw::oso::StorageContext& context);
	static bool EnsureDirectoryExists(const QString& dirPath);

public:
	CameraModule cameraModule;
	ReconnectModule reconnectModule;
	UIModule uiModule;
	ConfigManagerModule configManagerModule;
	AutoExposureModule autoExposureModule;
	AutoExposureModule autoExposureModule2;
	RuntimeInfoModule runtimeInfoModule;
	ImgSaveModule imgSaveModule;
	ImgProModule imgProModule;
	EliminateModule eliminateModule;
	PlcController plcController;
	UpsMonitorModule upsMonitorModule;
#ifdef BUILD_WITHOUT_HARDWARE
public:
	TestModule test_module;
#endif

};

template <class TypeCanToAssembly>
void Modules::checkFileExistAndFormat(const QString& path, const rw::oso::StorageContext& context)
{
	if (rw::rqw::RunEnvCheck::isFileExist(path))
	{
		if (!rw::rqw::RunEnvCheck::isFileFormatCorrectWithSafe<TypeCanToAssembly>(path, context))
		{
			auto assembly = context.load(path.toStdString());
			bool isMerge{ false };
			auto mergeAssembly = rw::oso::AssemblyMergeTool::Merge(TypeCanToAssembly(), *assembly, isMerge);
			if (isMerge)
			{
				ConfigManagerModule::saveAssemblyRobust(mergeAssembly, path.toStdString(), context, "Config");
			}
			else
			{
				ConfigManagerModule::saveAssemblyRobust(TypeCanToAssembly(), path.toStdString(), context, "Config");
			}
		}
	}
	else
	{
		ConfigManagerModule::saveAssemblyRobust(TypeCanToAssembly(), path.toStdString(), context, "Config");
	}
}

