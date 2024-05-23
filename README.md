# DragonRenderer
A soft raster renderer that uses GDI to draw on windows



### DragonRenderer构建说明：

**方法1：手动命令行构建**

1、CMake version >= 3.20

2、编译套件：目前仅测试  Visual Studio 17 2022

3、构建指令

- debug版本

    ```
    进入根目录 Crtl + R 输入cmd进入命令行
    生成配置 cmake -S . -B build -D CMAKE_BUILD_TYPE=Debug
    编译 cmake --build build --config Debug
    运行: .\build\Debug\bin\DragonRenderer.exe
    单元测试: 
    cd ./build
    test -C Debug -VV
    ```

- release版本

    ```
    进入根目录 Crtl + R 输入cmd进入命令行
    生成配置 cmake -S . -B build -D CMAKE_BUILD_TYPE=Delease 
    编译 cmake --build build --config Release
    运行: .\build\Release\bin\DragonRenderer.exe
    单元测试: 
    cd ./build
    ctest -C Release -VV
    ```



**方法2：利用vscode的cmake插件，利用CMakePresets预设进行构建**

确保前提条件：安装visual stdio 2022 以及c++编译套件、cmake >= 3.20

如下图：

![image-20240523221510925](./README.assets/image-20240523221510925.png)



**编码规范：** 遵循Google C++代码规范

**Git 提交记录规范格式说明：**

```
[<commit-type1>](<scope>):<subject>
1、xxxxxx
2、yyyyyy
3、zzzzzz

[<commit-type2>](<scope>):<subject>
1、xxxxxx
2、yyyyyy
3、zzzzzz
...
```

**commit-type表示提交类型（必选），分类如下：**

- feature
    新功能
- fix
    修复bug，可以是QA的，也可以是自己发现的
- docs
    添加 or 修改文档等
- refactor
    重构（不是新增功能，也不是修复bug）
- perf
    优化性能、体验等
- test
    增加单元测试、集成测试等
- chore
    构建过程的变动
- revert
    回滚到上一版本
- merge
    代码合并



**scope表示此次commit影响的范围（可选）！**

比如：1、某文件夹  2、界面层  3、模型层  4、某个库  等等



**subject表示此次commit提交的目的，一段简短的概述（可选）！**



**举个例子：**

```
[fix](application.cc文件): 忘记初始化某成员变量
1、补充成员变量main_window_width_的初始化
2、补充成员变量main_window_height_的初始化

[chore](application文件夹、根CMakeLists.txt): application单独编译成库，解耦
1、单独编译application文件夹，添加独立cmake文件
2、移除根cmake对application头文件包含，只需要target_link_library即可
```

