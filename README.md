# DragonRenderer

DragonRenderer 是一个运行在 Windows 上的软件光栅化渲染器。它用 C++ 手写一套接近 OpenGL 风格的渲染 API 和核心管线，用来学习并验证图形学中的坐标变换、裁剪、光栅化、纹理采样、深度测试、模型加载、相机交互和渲染性能分析。

![DragonRenderer preview](README.assets/image-20240621144356189.png)

## 项目定位

这个项目不是只做一个固定 demo，而是在逐步演进成一个模块化、可测试、可度量的软件渲染框架。

当前重点：

- 软件光栅化渲染管线
- OpenGL-like 资源与状态接口
- 可拆分的渲染阶段：顶点、裁剪、透视除法、剔除、视口、光栅、片元输出
- FPS、帧耗时、阶段耗时和渲染负载统计
- 面向后续引擎化重构的架构文档和路线图

## 功能概览

- 直线和三角形光栅化
- 重心坐标插值
- 纹理加载与采样
- Alpha 混合
- 背面剔除
- Z-buffer 深度测试
- Sutherland-Hodgman 裁剪
- 透视相机和基础交互
- Lambert 光照 Shader
- 基于 Assimp 的模型加载实验
- 自动 smoke / benchmark 模式

## 项目结构

```text
engine/
  app/        Win32 应用窗口、消息循环、主循环、帧统计展示
  entry_point.cc
              稳定程序入口，调用 demo factory 后启动引擎宿主

render/
  camera/     相机抽象与透视相机
  layer/      Event、Layer、LayerStack 扩展点
  pipeline/   DrawCommand、PipelineScratch、裁剪、光栅化、管线数据
  resource/   Buffer、VAO、Texture、Image、FrameBuffer
  runtime/    FrameStats、ScopedTimer 等运行时诊断
  scene/      Model / Mesh 加载与提交
  shader/     Shader 基类与内置 Shader

demos/
  dinosaur/   恐龙模型示例 Layer，通过 demo factory 提供给 engine 注册

test/         第三方库、渲染输出、裁剪、深度、NDC、DrawCommand 校验
assets/       运行时资源
thirdparts/   第三方依赖
docs/         架构、路线图、性能日志、项目日志和文档治理
tools/        工程辅助脚本
```

## 文档入口

所有长期文档统一收敛在 [docs/README.md](docs/README.md)。

常用入口：

- [当前架构与数据流](docs/ARCHITECTURE.md)
- [引擎化重构路线图](docs/ENGINE_REDESIGN_ROADMAP.md)
- [性能日志](docs/PERFORMANCE_LOG.md)
- [项目与开发日志](docs/PROJECT_LOG.md)

根目录不再维护重复的架构文档或开发日志正文，避免同一事实出现多个版本。

## 环境要求

- Windows
- Visual Studio 17 2022
- CMake >= 3.20
- PowerShell，用于 UTF-8 编码检查

当前项目主要在 Windows + Visual Studio 2022 环境下自测。

## 构建与运行

Debug 构建：

```powershell
cmake --preset Debug
cmake --build --preset x64-Windows-Build-Debug
.\build\Debug\bin\DragonRenderer.exe
```

Debug 版本适合断点调试和状态排查，但软件光栅化在 Debug 下交互帧率会明显偏低。如果主要想体验相机交互和渲染效果，建议使用 Release。

Release 构建：

```powershell
cmake --preset Release
cmake --build --preset x64-Windows-Build-Release
.\build\Release\bin\DragonRenderer.exe
```

自动 smoke / benchmark：

```powershell
.\build\Release\bin\DragonRenderer.exe --smoke 120
.\build\Release\bin\DragonRenderer.exe --benchmark 600
```

`--smoke` 和 `--benchmark` 会运行指定帧数后自动退出，并在控制台输出平均帧耗时、Update / Render / Present 分段耗时、管线阶段耗时和基础渲染负载计数。

## 测试

```powershell
ctest --test-dir build/Debug -C Debug --output-on-failure
ctest --test-dir build/Release -C Release --output-on-failure
```

也可以使用 CMake Presets：

```powershell
ctest --preset x64-Windows-Test-Debug
ctest --preset x64-Windows-Test-Release
```

## 工程约束

- 项目自有文本文件必须是 UTF-8。
- MSVC 构建启用 `/utf-8`。
- 默认构建目标会运行 `check_utf8`。
- CTest 中包含 `utf8_encoding`。
- 第三方依赖、生成物、二进制资源和图片资源不纳入项目文本编码策略。

更多规则见 [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md#encoding-policy)。

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

Unless otherwise noted, third-party dependencies, assets, course/tutorial materials, and externally sourced resources remain under their original licenses.
