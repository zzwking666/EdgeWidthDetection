#include "ConfigManagerModule.hpp"

#include "Modules.hpp"
#include "Utilty.hpp"

bool ConfigManagerModule::build()
{
	storeContext = std::make_unique<rw::oso::StorageContext>(rw::oso::StorageType::Xml);

    if (!loadConfigSafe(globalPath.EdgeWidthDetectionConfigPath,
        edgeWidthDetectionConfig, "EdgeWidthDetectionConfig"))
    {
        qWarning() << "使用默认 EdgeWidthDetection 配置";
    }
    if (!loadConfigSafe(globalPath.setConfigPath,
        setConfig, "SetConfig"))
    {
        qWarning() << "使用默认 SetConfig";
    }

	return true;
}

void ConfigManagerModule::destroy()
{
	storeContext->saveSafe(edgeWidthDetectionConfig, globalPath.EdgeWidthDetectionConfigPath.toStdString());
	storeContext->saveSafe(setConfig, globalPath.setConfigPath.toStdString());
	storeContext.reset();
}

void ConfigManagerModule::start()
{
	
}

void ConfigManagerModule::stop()
{

}
