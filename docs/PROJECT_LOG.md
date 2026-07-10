# DragonRenderer Project Log

This file is the unified project and development log for DragonRenderer.

Use this file for project-management decisions, documentation governance, and historical development notes. For measurable rendering performance entries, use [PERFORMANCE_LOG.md](PERFORMANCE_LOG.md). For target architecture and staged refactor planning, use [ENGINE_REDESIGN_ROADMAP.md](ENGINE_REDESIGN_ROADMAP.md). For current architecture diagrams and data flow, use [ARCHITECTURE.md](ARCHITECTURE.md).

## Project Management Entries

## 2026-07-10 - Adaptive Interaction Resolution Scaling

Decision:

- Upgrade interaction-time render-surface scaling from fixed active scale to adaptive feedback.
- Use previous-frame render time and rasterized-fragment coverage as the pressure signals.
- Treat `WindowConfig::interactive_render_scale` as the minimum allowed interaction scale rather than the mandatory active scale.
- Add `interactive_target_render_ms`, `interactive_target_coverage`, and `interactive_scale_step` to `WindowConfig`.
- Configure the dinosaur demo with minimum `0.45` scale, `8 ms` target render time, `0.16` target fragment coverage, and `0.1` scale step.

Rationale:

- The goal is stable interaction frame time, not always-low image quality.
- When the visible model occupies a small part of the screen, full resolution can be retained.
- When the visible workload grows near-camera, the renderer can reduce resolution progressively until it falls back under the configured pressure budget.
- Rasterized-fragment coverage is an available software-renderer proxy for screen occupancy until a real render-queue bounds or visibility system exists.

## 2026-07-10 - Progressive Interaction Resolution Recovery

Decision:

- Re-enable interaction-time render-surface scaling in the dinosaur demo, but make it progressive instead of fixed-low-resolution.
- Add `WindowConfig::interactive_recovery_steps` so the Win32 host can restore internal render size in discrete steps after interaction stops.
- Configure the dinosaur demo to use `0.6` interaction scale, `480 ms` recovery duration, and 4 recovery steps.
- Show the current internal render target size in the window title while the app is below full resolution.

Rationale:

- Many render tools deliberately reduce quality while the user is moving the view, then refine after interaction stops.
- The previous `0.35` fixed scale was too aggressive and did not communicate its state clearly.
- Discrete recovery steps avoid rebuilding the DIB/back buffer every frame while still giving the user progressive refinement.

## 2026-07-10 - Disable Visible Interaction Downscale In Dinosaur Demo

Decision:

- Stop enabling fixed-ratio interaction-time render-surface scaling in the dinosaur demo by default.
- Keep the `WindowConfig` mechanism available for future applications that explicitly choose the quality/performance tradeoff.
- Clear held keyboard and mouse state on Win32 focus loss so a missed key-up or mouse-up cannot keep the app in an interaction state.
- Extend `input_state` coverage for held-state reset.

Rationale:

- A fixed `0.35` internal render scale is too visible during ordinary camera movement, especially when the model is far away and the full-resolution render cost is already lower.
- Interaction performance should be improved through raster/output streaming, tile culling, and adaptive policies rather than forcing a visibly blurry frame whenever the user moves.
- Losing focus while holding input can leave stale held state behind if the corresponding release message is missed.

## 2026-07-10 - Chunked Raster Output Merge

Decision:

- Add a chunked raster-output path through `RasterTool::RasterizeChunked`.
- Let `Renderer::RunRasterOutputStage` flush raster chunks directly into perspective recovery, fragment shading, depth test, blending, and framebuffer writes.
- Keep `PipelineScratch::raster_outputs` as reusable scratch storage, but treat it as a bounded chunk buffer instead of a whole-draw fragment accumulator.

Rationale:

- Close-camera views and large triangles can create bursty fragment output.
- A draw-sized fragment vector couples rasterization and output merge too tightly for future tile or parallel execution.
- Bounded chunk flushing lowers peak temporary storage and gives the renderer a cleaner streaming boundary before a full tile-based raster path exists.

## 2026-07-09 - Close-Camera Raster Pressure Mitigation

Decision:

- Clamp triangle raster bounding-box scans to the active framebuffer viewport.
- Add `raster_viewport_clamp` coverage for fully offscreen and oversized triangles.
- Lower the dinosaur demo's interaction render scale from `0.5` to `0.35`.
- Extend the dinosaur demo's interaction recovery window from `180 ms` to `360 ms`.
- Enable back-face culling in the dinosaur demo.

Rationale:

- Near-camera triangles can cover a large part of the framebuffer and create sudden raster/fragment bursts.
- The rasterizer should not scan pixels outside the render target.
- The dinosaur model is a closed mesh demo, so back-facing triangles are not useful for the default interaction view.
- A longer interaction recovery window avoids quickly bouncing back to full resolution while the user is still adjusting the view.

## 2026-07-09 - Interactive Scaling Trigger Narrowing

Decision:

- Keep passive mouse movement as input activity for cursor position and delta tracking.
- Stop using passive mouse movement to refresh the interactive render-surface scaling window.
- Trigger interactive render-surface scaling only while a keyboard key is held or a mouse button is held.

Rationale:

- Moving the cursor without pressing a button does not change the current camera in the demo.
- Lowering resolution for passive cursor movement makes the quality/performance policy visible when the user is not actually interacting with the rendered scene.
- The platform host should distinguish input observation from scene-changing interaction intent.

## 2026-07-09 - Interactive Render Surface Scaling

Decision:

- Add optional interaction-time render-surface scaling to `WindowConfig`.
- Keep the default disabled so engine users do not inherit a hidden quality/performance tradeoff.
- Let the dinosaur demo explicitly enable a 0.5 internal render scale during active input and recover after a short idle window.
- Add `Renderer::ResizeRenderTarget` so the platform host can resize the software back buffer without recreating the whole renderer.
- Present scaled internal frames through Win32 `StretchBlt` while preserving the configured external window size.

Rationale:

- Mouse interaction was still expensive because every camera movement re-rendered the full 1200x900 software back buffer.
- Input coalescing reduces event-driven update pressure, but the software rasterizer still needs a lower-cost interactive path until tile culling, cached transforms, or deeper pipeline optimizations exist.
- This is an application policy rather than a renderer invariant: different demos or user applications should be able to choose their own interaction quality/performance balance.

## 2026-07-09 - Per-Frame Input State

Decision:

- Add `InputState` as a per-frame input snapshot for keyboard and mouse state.
- Change `WindowsApplication` to drain all pending Win32 messages once per frame.
- Stop dispatching `WM_MOUSEMOVE` directly into `Renderer::OnEvent` from `WndProc`.
- Add `Renderer::OnInput` and `Camera::OnInput` so camera input is consumed once per frame.
- Keep the older event path available while platform input is migrated toward the new runtime boundary.

Rationale:

- High-frequency mouse messages should not synchronously drive renderer or camera work.
- The platform layer should collect OS input, while renderer/camera systems consume an engine input state.
- This is a stepping stone toward a dedicated `InputSystem` and `Runtime` loop.

## 2026-07-09 - Application Window Config Registration

Decision:

- Remove hard-coded window title and size from `engine/entry_point.cc`.
- Add `ApplicationConfig`, `WindowConfig`, `ApplicationConfigRegistry`, and `ApplicationConfigAutoRegistrar`.
- Let application/demo modules register their desired window configuration through the engine API.
- Let `WindowsApplication` read the application config during initialization.
- Keep `ApplicationConfig` as a multi-window-capable list while the current Win32 host consumes the primary window.

Rationale:

- The entry point should not encode app-specific window policy.
- The dependency direction should be user application to engine configuration, not engine to user application assumptions.
- A window config list gives the future platform/window manager a clear place to support multiple windows without reversing ownership later.

## 2026-07-09 - Framework Documentation Sync Rule

Decision:

- Treat documentation synchronization as part of the definition of done for framework-level changes.
- Require architecture, roadmap, project-log, and performance-log updates whenever a code change alters the corresponding facts.
- Add a repository PR checklist so future framework changes explicitly confirm documentation synchronization.

Rationale:

- DragonRenderer is being refactored as a framework, so the documentation is part of the framework contract.
- Architecture diagrams and roadmap status become misleading if they lag behind module or lifecycle changes.
- Keeping code and documents in the same commit makes decisions easier to audit and safer to continue later.

## 2026-07-09 - Static Layer Auto-Registration

Decision:

- Replace the explicit entry-point call to `RegisterDemoLayers(...)` with static layer auto-registration.
- Rename the registry boundary from `DemoLayerRegistry` to the engine-owned `LayerRegistry`.
- Let `WindowsApplication` attach registered layers after creating and initializing the renderer.
- Build the current dinosaur demo as an object target so its static registration unit is guaranteed to be linked into `DragonRenderer.exe`.

Rationale:

- The executable entry point should not know that demos exist.
- Demo modules should contribute themselves to the engine through extension registration.
- Static libraries can drop object files that are only needed for static initialization; using an object target keeps registration deterministic.
- This makes the current entry point a true engine host entry: create application, initialize, run.

## 2026-07-09 - Demo Registry and Application Abstraction

Decision:

- Replace the single `CreateDemoLayer(Renderer*)` demo factory with an engine-owned `DemoLayerRegistry`.
- Let demo modules implement `RegisterDemoLayers(DemoLayerRegistry&)` and register their own layer factories.
- Turn `Application` into a platform-neutral lifecycle interface.
- Move the current Win32/GDI host into `engine/platform/win32/WindowsApplication`.

Rationale:

- The engine should own the extension point, while demos should only contribute business/demo logic through registration.
- The executable entry point should not include concrete demo layer headers or know which demo class is active.
- The application lifecycle needs to be reusable beyond the current Windows window host.
- Keeping Win32 details behind `WindowsApplication` makes later runtime, headless benchmark, or alternate platform hosts easier to introduce.

## 2026-07-08 - Documentation Governance

Decision:

- Treat `docs/` as the canonical home for long-lived engineering documents.
- Keep the repository root focused on `README.md`, `LICENSE`, CMake files, source folders, assets, tests, and tools.
- Remove the duplicate root `ARCHITECTURE.md` body because [docs/ARCHITECTURE.md](ARCHITECTURE.md) is the authoritative architecture document.
- Merge the old root `DEVLOG.md` content into this file as historical development entries.
- Create [docs/README.md](README.md) as the documentation map and governance entry.

Rationale:

- The previous layout had architecture content both in the root and under `docs/`, which made it unclear which file represented current truth.
- Development notes and project-management records should not compete with each other.
- A single documentation index makes future architecture, roadmap, and performance updates easier to place.

Current policy:

- Current facts go to [ARCHITECTURE.md](ARCHITECTURE.md).
- Future direction goes to [ENGINE_REDESIGN_ROADMAP.md](ENGINE_REDESIGN_ROADMAP.md).
- Measured numbers go to [PERFORMANCE_LOG.md](PERFORMANCE_LOG.md).
- Cross-document and project-management decisions go to this file.
- Historical root notes remain below in this same unified log.

## 2026-07-08 - Log Consolidation

Decision:

- Merge the previous development log and the newer project-management log into this single `PROJECT_LOG.md`.
- Remove the separate historical-log filename so older notes remain part of the formal project record instead of looking deprecated.
- Keep performance measurements in [PERFORMANCE_LOG.md](PERFORMANCE_LOG.md) and architecture facts in [ARCHITECTURE.md](ARCHITECTURE.md).

Rationale:

- The project history is useful context for future architecture work.
- A single project log is easier to scan than separate management and historical log files.
- Avoiding archive-style naming keeps early learning records connected to the current renderer evolution.

## 2026-07-08 - Historical Log Encoding Repair

Decision:

- Rebuild the historical development entries from the clean `ad23012:DEVLOG.md` Git blob.
- Replace the unreadable text introduced during the earlier log merge with valid UTF-8 Chinese.
- Keep the older development notes inside this unified project log.

Rationale:

- The original `DEVLOG.md` content in Git history was already valid UTF-8.
- The unreadable text came from an incorrect decode path during document consolidation, not from the original notes.
- Restoring from the original blob preserves the project history without manually rewriting the old entries.

## 2026-07-09 - Demo Boundary Split

Decision:

- Move the built-in dinosaur demo out of `render/` and into `demos/dinosaur/`.
- Build the demo as a separate `DinosaurDemo` static library.
- Link `DragonRenderer.exe` against both `Render` and `DinosaurDemo`.
- Remove `Renderer::InitLayer()` and move demo registration out to the executable layer.

Rationale:

- `Render` should be the renderer framework core, not the owner of a specific demo scene.
- Demo code should depend on renderer APIs; renderer code should not include or instantiate demo layers.
- This is a stepping stone toward a future demo registry and cleaner app/runtime split.

## 2026-07-09 - Demo Entry Factory

Decision:

- Add `demos/demo_layer_factory.h` as the stable boundary between the executable entry point and demo code.
- Let `demos/dinosaur/dinosaur_demo.cc` implement `CreateDemoLayer(Renderer*)`.
- Keep `entry_point.cc` generic: it initializes the application, asks the linked demo module for a layer, and starts the run loop.

Rationale:

- Users adding or replacing demos should work in the demo/business layer instead of editing the Windows entry point.
- The executable can stay as a stable host while concrete demo classes remain inside their own demo module.
- This prepares the project for a future demo registry without forcing that abstraction into the current code immediately.

## 2026-07-09 - Engine Host Split

Decision:

- Move `entry_point.cc` into `engine/`.
- Move the Win32 `Application` shell into `engine/app/`.
- Move `Event`, `Layer`, and `LayerStack` into `render/layer/` as renderer extension points.
- Add a `DragonEngine` object-library target for the executable host code.
- Keep `Render` focused on renderer core code and `DinosaurDemo` focused on demo/business code.

Rationale:

- The executable entry point and application host are engine infrastructure, not demo code and not render-core code.
- `Render` should provide rendering APIs and layer extension points without owning the OS application shell.
- Keeping demo creation behind `CreateDemoLayer(Renderer*)` lets users focus on business/demo layer code while the engine host remains stable.

## Historical Development Entries

The entries below were migrated from the old root-level `DEVLOG.md` and normalized as valid UTF-8 Chinese text. They remain part of the continuous project record.

### 开发过程日志

此MarkDown专门用于写一些开发过程的日志记录，防止前一天想到的东西，第二天忘记了



------------

2024-5-22

1、添加直线光栅化Brensenham算法并测试（ok）

2、增加光栅化直线的单元测试工程 test/line_rasterize_test.cc（ok）

注意：

> 引入了一个暴力光栅直线的接口，
>
> 但是在 k > 1的时候生成的和Brensenham算法就是有所区别，0 <=k<=1 就是完全一样
>
> 为了学习，后面单独给出这部分的测试用例！
>
> TODO： 后续有空还想给这两个算法做时间基准测试

 因为引入新的Pixel类，看看以前的绘制一个点有没有接口可以优化的



2024-5-23

1、尝试添加vs2022构建套件的CMakePreset文件，方便使用vscode调试的正常开展（ok）

2、添加DrawPixel接口，只传入Pixel即可渲染（ok）

3、添加ctest针对直线光栅化的单元测试（ok）

4、Brensenham补充完善，添加对颜色属性的差值（ok）

5、集成第三方数学库glm，提供常用的向量和矩阵运算（ok）



2024-5-24

1、给glm的测试多补充一些案例，熟悉下基本使用（ok）

2、准备改造单元测试，引入gtest，没有断言测试起来太麻烦了（TODO）

3、添加三角形光栅化的接口（ok）

4、添加三角形重心坐标公式插值接口（ok）

5、朴素直线光栅化解耦，添加插值（ok）

6、将glm作为接口库使用，依赖他的目标取消include_directories属性设置（ok）

7、利用glm优化现有的数据结构和接口（ok）

8、将通用的Color、Pixel等文件也抽象成接口库（Failed，原因不明，先不试了）



2024-5-25

1、准备改造单元测试，引入gtest，没有断言测试起来太麻烦了（TODO）

2、写三角形利用重心坐标公式插值的博客（ok）

3、引入stb_image，并添加测试（ok）

4、准备图片类，用于存储解码的图片数据（ok）

5、新建资产文件夹assert，在根目录下，并为其创建接口库（ok）



2024-5-26

1、准备改造单元测试，引入gtest（TODO）

2、引入混合，利用alpha绘制半透明的效果（ok）l

3、引入纹理，最近邻uv采样方法（ok）

4、增加主窗体默认居中显示（ok）

5、FrameBuffer类调整red和blue的顺序（ok）

6、解决这个颜色序列化不是bgra的问题，规定所有的数据都得按照bgra存放（ok）



2024-5-27

1、引入纹理采样的双线性插值（ok）

2、引入Wrap的Repeat和Mirror方式（ok）



2024-5-28

1、尝试利用glm提供的MVP矩阵的接口，进行编写测试（ok）

2、各种矩阵的推导的博客分享

- 平移、旋转、缩放矩阵推导（TODO）
- 任意轴旋转矩阵推导（TODO）
- 视图变换矩阵推导（TODO）
- 正交投影矩阵推导（TODO）
- 透视投影矩阵推导（TODO）



2024-5-29

1、各种矩阵的推导的博客分享（ok）



2024-5-30

1、仿照OpenGL的接口，引入VBO和VAO数据结构（ok）

2、Render类添加bind相关对象的接口（ok）

3、提供Singer VBO的数据测试接口（ok）



2024-5-31

1、添加Pipeline相关的数据结构（ok）

2、添加绘制接口（ok）

3、封装Shader抽象类，提供vertexshader和fragmentshader接口（ok）

4、提供默认Shader类（ok）

5、测试能够显示三角形（ok）



2024-6-1

1、解决编译过程中提示的代码页的警告，只要把文件编码保存成UTF8-BOM即可！

2、删除大量之前的老接口，并且删除Pixel结构体，更改pixel.h为pipeline_datastruct.h

3、优化一下主循环的接口



2024-6-2

1、编写Suntherland hodgman剪裁算法（仅针对三角形绘制），并在透视除法之前引入（ok）

2、编写针对Suntherland hodgman算法博客（ok）

3、完善Suntherland hodgman剪裁算法（补充针对直线绘制）（ok）



2024-6-3

1、实现背面剔除相关接口和设置（ok）

2、实现z-buffer机制的深度测试（ok）

3、透视矫正的原理学习和公式推导，以及接口开发（TODO）



2024-6-7

1、引入透视矫正相关（ok）



2024-6-12

1、封装摄像机类（ok）



2024-6-13

1、封装Lambert光照模型渲染，并提供测试（ok）

2、子类Shader使用父类Shader的GetVector存在风险，因为一律都是拷贝4个float（TODO）

可以考虑拆分接口，提供2个float、3个、4个等等！

3、引入Assimp第三方模型加载库（ok）

4、添加assimp库的单元测试（ok）

5、封装Mesh类和Model类，用于加载模型（TODO）





2024-6-21

1、重构CMake工程架构

- src去除（ok）
- 第三方库引入重构（ok）
- 资产assert编译增加拷贝到exe运行目录下，这样程序方便使用相对路径引用资产

2、分层封装，重构现有类

目前的代码问题分析：

- 不能兼容宽窄字符串，请使用兼容的Win32接口（ok）
- 优化Application命名（ok）

- 渲染DrawCall接口的封装不够优雅
- 没有类似场景的概念，不方便做切换的测试
- 交互卡顿，尤其是距离模型较近的时候，帧数太低



2024-7-17

1、目前将Application并入Render文件夹，后续再考虑如何拆分（ok）

2、考虑重新封装Application，Application将消息封装成事件回调通知Renderer（TODO）

3、实际上应该是Application依赖Render，Render被Application通知，它不需要认识Application（TODO）

4、相机抽象基类，提供透视相机继承重写（ok）


2025-2-16
1 README文档修改, 修复release版本的运行命令
2 优化Application类的成员变量命名
3 完善Application类的单例实现, 改为局部静态变量方式
4 完善AbstracCamera类的相关内容, KeyCode引入额外的命名空间, MOVE_STATE从单独int变成enum
5 优化Render类的接口封装
6 优化TestLayer层的接口封装
7 增加ESC键退出
8 优化VertexArrayObject成员命名
9 优化Pipeline_data的原有配置选项, 全部改为enum, 提高可读性
