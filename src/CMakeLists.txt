# src/CMakeLists.txt


add_subdirectory(core)
add_subdirectory(application)
add_subdirectory(render)

add_executable(DragonRenderer "win_main.cc")
target_link_libraries(DragonRenderer  Application Render assets_lib CoreLib thirdpart_lib_glm)
#target_include_directories(DragonRenderer PRIVATE ${CMAKE_SOURCE_DIR}/src/core)

# TODO 可优化为接口库，为所有的可执行程序或者测试程序附加
target_link_options(DragonRenderer PRIVATE "/subsystem:console" "/entry:wWinMainCRTStartup")
# or set_target_properties(DragonRenderer PROPERTIES LINK_FLAGS "/subsystem:console /entry:wWinMainCRTStartup")

