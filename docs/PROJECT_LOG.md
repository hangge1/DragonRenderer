# DragonRenderer Project Log

This file is the unified project and development log for DragonRenderer.

Use this file for project-management decisions, documentation governance, and historical development notes. For measurable rendering performance entries, use [PERFORMANCE_LOG.md](PERFORMANCE_LOG.md). For target architecture and staged refactor planning, use [ENGINE_REDESIGN_ROADMAP.md](ENGINE_REDESIGN_ROADMAP.md). For current architecture diagrams and data flow, use [ARCHITECTURE.md](ARCHITECTURE.md).

## Project Management Entries

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

## Historical Development Entries

The entries below were migrated from the old root-level `DEVLOG.md` and are kept as part of the continuous project record.

Note: parts of the older content already contained mojibake before this merge. The text is preserved instead of silently rewritten so historical material remains traceable.

# 开发过程日志

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
