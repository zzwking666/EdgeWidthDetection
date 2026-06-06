# CLAUDE.md

此文件为 Claude Code (claude.ai/code) 在此仓库中工作时提供指导。

## 构建命令

```sh
# 首次使用 — 克隆 RW_UL 依赖库
./ConfigProject.bat

# 配置（无硬件测试模式 — 最常用的开发模式）
cmake --preset vs2022-NoHardware

# 配置（带硬件 — 相机、PLC、CUDA）
cmake --preset vs2022

# 构建
cmake --build --preset vs2022-debug-NoHardware   # debug, 无硬件
cmake --build --preset vs2022-release-NoHardware  # release, 无硬件
cmake --build --preset vs2022-debug               # debug, 带硬件
cmake --build --preset vs2022-release             # release, 带硬件

# 初次配置后只构建主目标
cmake --build out/build/vs2022-NoHardware --target EdgeWidthDetection
```

**前置条件:** Visual Studio 2022（或 2019）、Qt6、vcpkg（需设置 `VCPKG_ROOT` 环境变量）。`vs2022-NoHardware` 预设会设置 `BUILD_WITHOUT_HARDWARE=true`，这是本地开发的默认选择。

## 代码风格

- Tab 缩进，4 空格宽度（见 `.editorconfig`）
- UTF-8 编码
- 大括号与控制语句同行，左大括号前加空格
- 括号内不加空格；逗号后和二元运算符两侧加空格
- 使用 `#pragma once` 作为头文件保护
- C++17 标准（`CMAKE_CXX_STANDARD 17`）
- 所有注释和对外沟通用**中文**

## 架构

### 模块生命周期

所有功能组件都实现 `IModule<TBuildError>` 接口（定义在 `include/Modules/IModule.hpp`）：

```cpp
template<class TBuildError = bool>
class IModule {
    virtual TBuildError build() = 0;   // 初始化资源、检测硬件
    virtual void destroy() = 0;        // 释放资源
    virtual void start() = 0;          // 开始运行
    virtual void stop() = 0;           // 暂停运行
};
```

`TBuildError` 类型因模块而异：`bool` 表示简单的成功/失败，`void` 表示无错误返回，`std::vector<BuildError>` 用于多错误场景（如 `CameraModule` 按相机报告错误）。

### 单例中枢 — `Modules`

`Modules`（`include/Modules/Modules.hpp/.cpp`）是**全局单例**，职责如下：

1. 以公开成员字段持有所有子模块
2. 按依赖顺序为每个模块调用 `build()`（顺序：配置 → 相机 → 图像处理 → UI → 其他）
3. 在 `connect()` 中完成所有模块间的 Qt 信号槽连接
4. 统一调用所有模块的 `start()` / `stop()`

**不要在单个模块内部添加模块间的连接逻辑。** 所有跨模块连接都应放在 `Modules::connect()` 中。

### 子模块

| 模块 | 头文件 | 职责 |
|------|--------|------|
| `ConfigManagerModule` | `ConfigManagerModule.hpp` | 加载/保存配置 XML（EdgeWidthDetection + SetConfig），包含 `loadConfigSafe<T>()` |
| `CameraModule` | `CameraModule.hpp` | 海康威视/度申相机初始化、帧采集、重连 |
| `ImgProModule` | `ImgProModule.hpp` | 图像处理管线（预处理 + 检测），持有 `ImageProcessingModule` |
| `UIModule` | `UIModule.hpp` | 主窗口（`EdgeWidthDetection`）+ 产品设置对话框（`DlgProductSet`） |
| `PlcController` | `PlcController.hpp` | Modbus PLC 通信，读写寄存器值 |
| `RuntimeInfoModule` | `RuntimeInfoModule.hpp` | 运行时状态追踪（调试/运行/停止）、废品计数、PLC 信息轮询 |
| `ImgSaveModule` | `ImgSaveModule.hpp` | 通过 `ImageSaveEngine` 异步保存图像到磁盘 |
| `EliminateModule` | `EliminateModule.hpp` | 缺陷剔除逻辑，产品判定的优先级队列 |
| `ReconnectModule` | `ReconnectModule.hpp` | 监控相机/板卡连接状态，断连时触发重连 |

### 配置系统（`.oso` → 代码生成）

配置结构在 `osoFile/` 中使用 `.oso` DSL 文件定义（如 `SetConfig.oso`、`EdgeWidthDetection.oso`）。构建系统通过 `SerializedCodeGenerator` cmake 模块将这些文件编译为 `osoInclude/` 下的 C++ 类。

核心模式 — **版本兼容的配置加载**（`ConfigManagerModule.hpp:34`）：

```cpp
template<class ConfigType>
bool loadConfigSafe(const QString& path, ConfigType& outConfig, const QString& configName);
```

当配置文件 XML 缺少字段时（例如 `.oso` 中新加了字段），模板会捕获 `std::runtime_error`，通过 `AssemblyMergeTool::Merge` 将旧数据与新的默认值合并，保存合并后的文件，然后重试。**加载配置文件时一律使用此方法，不要直接用 `loadSafe()`。**

新增配置字段的步骤：
1. 编辑 `.oso` 文件 → 添加 `字段名 : 类型 : default = 默认值;`
2. 重新构建 — 代码生成器会自动重新生成头文件
3. `loadConfigSafe` 模板会自动处理向后兼容的合并

### 线程模型

- 主线程：Qt 事件循环（UI）
- `CameraPassiveThread`：每个相机一个独立线程，发送 `frameCaptured1` 信号
- `DetachPLCListenThread`：后台 Modbus 轮询读取 PLC 寄存器
- `DetachUtiltyThread`：轮询 PLC 信息和统计数据，通过 `Qt::QueuedConnection` 向 UI 发更新
- `DetachDefectThread`：异步缺陷剔除判定
- `ImageSaveEngine`：异步图像写入，可配置线程数

**规则：** 跨线程更新 UI 的信号必须使用 `Qt::QueuedConnection`。发向图像处理模块的信号使用 `Qt::DirectConnection`（处理逻辑直接在相机线程执行）。

### `BUILD_WITHOUT_HARDWARE` 模式

当此 cmake 选项为 `true` 时：
- `TestModule` 替代真实相机，从磁盘推送测试图像
- CUDA/Halcon/相机 SDK 依依赖仍会链接但不会初始化
- 用于在没有工业硬件的开发机器上进行 UI 开发和算法测试

预设 `vs2022-NoHardware` 设置了 `BUILD_WITHOUT_HARDWARE=true`。

## 关键三方依赖

- **RW_UL**（`ThirdRep/RW_UL/`）— 核心工具库（从 `github.com/WIND-ROAD-RUN/RW_UL` 克隆），提供：
  - `rw::oso` — 对象序列化（ObjectStoreAssembly），XML/JSON 存储
  - `rw::rqw` — 相机硬件抽象层、图像保存引擎、运行环境检查
  - `rw::imgPro` — 图像处理上下文和工具
  - `rw::hoem` — Modbus 设备抽象
  - `rw::dsl` — 线程安全数据结构（如 ThreadSafeHeap）
- 相机 SDK：MVS（海康威视）、DS（度申）
- 可选：Halcon、CUDA 11.3/TensorRT 8.6、ONNX Runtime

## UI 注意事项（来自 `.github/copilot-instructions.md`）

- `QCheckBox` 样式应简洁、视觉明显，文字使用高对比度的亮色（不要用暗色/灰色）
