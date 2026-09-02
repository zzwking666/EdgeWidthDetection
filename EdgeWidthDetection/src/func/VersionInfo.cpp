#include "VersionInfo.hpp"

#include <QCoreApplication>
#include <QFile>

#include "BuildVersion.hpp"

namespace ewd {

QString getDisplayVersion()
{
	// 优先读取 exe 同目录的 build.version（每次构建后由 cmake 拷贝更新）
	const QString filePath = QCoreApplication::applicationDirPath() + "/build.version";
	QFile file(filePath);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		const QString content = QString::fromUtf8(file.readAll()).trimmed();
		if (!content.isEmpty())
		{
			return content;
		}
	}

	// 文件缺失或为空：回退到编译进二进制的版本号，保证一定有版本可显示
	return getEmbeddedVersion();
}

QString getEmbeddedVersion()
{
	return QString::fromLatin1(FULL_VERSION_STRING);
}

}
