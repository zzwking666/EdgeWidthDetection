#include "ConfigManagerModule.hpp"

#include <filesystem>

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
	saveEdgeWidthDetectionConfigSafe();
	saveConfigSafe();
	storeContext.reset();
}

void ConfigManagerModule::start()
{

}

void ConfigManagerModule::stop()
{

}

void ConfigManagerModule::rotateBackups(const std::filesystem::path& filePath, int maxBackups)
{
	// 轮转多代备份链：.bak → .bak2 → .bak3
	// saveSafe 内部会将当前主文件复制为 .bak，所以先把旧 .bak 往更老的代推
	for (int i = maxBackups - 1; i >= 0; --i)
	{
		auto srcSuffix = i == 0 ? std::string(".bak") : (".bak" + std::to_string(i));
		auto dstSuffix = ".bak" + std::to_string(i + 1);
		auto srcPath = filePath.string() + srcSuffix;
		auto dstPath = filePath.string() + dstSuffix;

		if (std::filesystem::exists(srcPath))
		{
			std::error_code ec;
			std::filesystem::copy_file(srcPath, dstPath,
				std::filesystem::copy_options::overwrite_existing, ec);
		}
	}
}

bool ConfigManagerModule::saveAssemblyRobust(const rw::oso::ObjectStoreAssembly& assembly,
	const std::filesystem::path& filePath,
	const rw::oso::StorageContext& context,
	const QString& configName)
{
	// 1. 多代备份轮转
	rotateBackups(filePath);

	// 2. 使用 saveSafe 保存（内部 .tmp → .bak → rename）
	if (!context.saveSafe(assembly, filePath))
	{
		qWarning() << configName << "saveSafe 写入失败:" << QString::fromStdString(filePath.string());
		return false;
	}

	// 3. 写入后验证：重新加载确认文件完整可读
	auto verifyAssembly = context.loadSafe(filePath);
	if (verifyAssembly)
	{
		// 结构验证通过
		return true;
	}

	// 4. 验证失败，尝试从 .bak 恢复
	qWarning() << configName << "保存后验证失败，尝试从 .bak 恢复";
	auto bakPath = filePath.string() + ".bak";
	if (std::filesystem::exists(bakPath))
	{
		std::error_code ec;
		std::filesystem::copy_file(bakPath, filePath,
			std::filesystem::copy_options::overwrite_existing, ec);
		if (!ec)
		{
			qWarning() << configName << "已从 .bak 恢复";
		}
		else
		{
			qWarning() << configName << ".bak 恢复失败";
		}
	}

	return false;
}

bool ConfigManagerModule::saveConfigSafe()
{
	if (!storeContext)
		return false;
	return saveAssemblyRobust(setConfig, globalPath.setConfigPath.toStdString(), *storeContext, "SetConfig");
}

bool ConfigManagerModule::saveEdgeWidthDetectionConfigSafe()
{
	if (!storeContext)
		return false;
	return saveAssemblyRobust(edgeWidthDetectionConfig, globalPath.EdgeWidthDetectionConfigPath.toStdString(), *storeContext, "EdgeWidthDetectionConfig");
}
