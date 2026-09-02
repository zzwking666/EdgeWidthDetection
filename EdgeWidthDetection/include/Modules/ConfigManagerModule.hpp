#pragma once

#include"IModule.hpp"
#include<QObject>
#include "oso_StorageContext.hpp"
#include "SetConfig.hpp"
#include "EdgeWidthDetection.hpp"
#include "oso_func.hpp"
#include <QDebug>

class ConfigManagerModule
	: public QObject, public IModule<bool>
{
	Q_OBJECT
public:
	bool build() override;
	void destroy() override;
	void start() override;
	void stop() override;
public:
	std::unique_ptr<rw::oso::StorageContext> storeContext{ nullptr };
public:
	cdm::EdgeWidthDetectionConfig edgeWidthDetectionConfig{};
	cdm::SetConfig setConfig{};

public:
	template<class ConfigType>
	bool loadConfigSafe(const QString& path, ConfigType& outConfig, const QString& configName);
public:
	/// 安全保存 SetConfig，含多代备份轮转与写入后验证，防止断电损坏
	bool saveConfigSafe();
	/// 安全保存 EdgeWidthDetectionConfig
	bool saveEdgeWidthDetectionConfigSafe();
	/// 带验证的安全保存核心（静态，可被初始化检查复用）
	static bool saveAssemblyRobust(const rw::oso::ObjectStoreAssembly& assembly,
		const std::filesystem::path& filePath,
		const rw::oso::StorageContext& context,
		const QString& configName);
private:
	/// 多代备份轮转（.bak → .bak2 → .bak3）
	static void rotateBackups(const std::filesystem::path& filePath, int maxBackups = 3);
};

template <class ConfigType>
bool ConfigManagerModule::loadConfigSafe(const QString& path, ConfigType& outConfig, const QString& configName)
{
	// 整体兜底：XML 解析、字段缺失、文件系统错误等任何异常都不允许抛穿导致程序闪退，
	// 而是返回 false，由调用方回退到默认配置继续运行
	try
	{
		// 尝试加载：主文件 → .bak（loadSafe 已内置，主文件缺失时也会自动尝试）→ .bak2 → .bak3
		auto tryLoad = [this](const QString& p) -> std::shared_ptr<rw::oso::ObjectStoreAssembly>
		{
			return storeContext->loadSafe(p.toStdString());
		};

		auto loadData = tryLoad(path);
		if (!loadData)
		{
			// loadSafe 已尝试过主文件和 .bak，这里继续尝试更老的备份
			const std::filesystem::path filePath = path.toStdString();
			for (int i = 2; i <= 3; ++i)
			{
				auto bakPath = QString::fromStdString(filePath.string() + ".bak" + std::to_string(i));
				loadData = tryLoad(bakPath);
				if (loadData)
				{
					qWarning() << configName << "从备份" << bakPath << "恢复";
					// 使用稳健保存将恢复的数据写回主文件
					ConfigManagerModule::saveAssemblyRobust(*loadData, filePath, *storeContext, configName);
					break;
				}
			}

			if (!loadData)
			{
				qWarning() << configName << "所有备份均无法加载:" << path;
				return false;
			}
		}

		try
		{
			outConfig = ConfigType(*loadData);
			return true;
		}
		catch (std::runtime_error&)
		{
			qWarning() << configName << "配置文件字段不匹配，尝试合并默认值...";

			rw::oso::ObjectStoreAssembly oldAssembly = *loadData;
			rw::oso::ObjectStoreAssembly newAssembly = ConfigType();
			rw::oso::AssemblyMergeTool::Merge(newAssembly, oldAssembly);

			if (ConfigManagerModule::saveAssemblyRobust(newAssembly, path.toStdString(), *storeContext, configName))
			{
				qDebug() << configName << "配置文件更新成功:" << path;

				// 合并后重新尝试构造
				try
				{
					outConfig = ConfigType(newAssembly);
					return true;
				}
				catch (...)
				{
					qWarning() << configName << "合并后仍无法解析，使用默认值";
				}
			}
			else
			{
				qWarning() << configName << "合并后保存失败:" << path;
			}
			return false;
		}
	}
	catch (const std::exception& e)
	{
		qWarning() << configName << "加载配置时发生异常:" << e.what() << "，使用默认配置:" << path;
		return false;
	}
	catch (...)
	{
		qWarning() << configName << "加载配置时发生未知异常，使用默认配置:" << path;
		return false;
	}
}
