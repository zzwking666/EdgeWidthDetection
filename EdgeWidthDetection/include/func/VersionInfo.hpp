#pragma once

#include <QString>

namespace ewd {

/// 获取用于显示的版本号：
/// 优先读取 exe 同目录下的 build.version 文件（每次构建后由 cmake 拷贝过去），
/// 文件缺失或内容为空时回退到编译期嵌入二进制的版本号
QString getDisplayVersion();

/// 编译期嵌入二进制的完整版本号（版本号_时间戳_git短哈希）
QString getEmbeddedVersion();

}
