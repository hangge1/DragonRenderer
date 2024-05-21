# DragonRenderer
A soft raster renderer that uses GDI to draw on windows



编码规范尽可能遵循：Google C++代码规范

Git 提交记录规范说明：

```
[<commit-type1>](<scope>):<subject>
1、
2、
3、
[<commit-type2>](<scope>):<subject>
1、
2、
3、
...
```

**commit-type表示提交类型，必选项，分类如下：**

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



**scope表示此次commit影响的范围，是可选项！**

比如：1、某文件夹  2、界面层  3、模型层  4、某个库  等等



**subject表示此次commit提交的目的，是一段简短的说明即可，是必选项！**



**举个例子：**

```
[fix](application.cc文件): 忘记初始化某成员变量
1、补充成员变量main_window_width_的初始化
2、补充成员变量main_window_height_的初始化

[chore](application文件夹、根CMakeLists.txt): application单独编译成库，解耦
1、单独编译application文件夹，添加独立cmake文件
2、移除根cmake对application头文件包含，只需要target_link_library即可
```

