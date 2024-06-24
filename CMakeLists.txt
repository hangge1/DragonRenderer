# CMakeLists.txt

cmake_minimum_required(VERSION 3.20)

enable_testing()

# 默认Release配置
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 设置可执行文件、库的输出路径
set(GENERATE_DIR ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})

set(BIN_DIR ${GENERATE_DIR}/bin)
set(LIB_DIR ${GENERATE_DIR}/lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR})

# 针对Debug和Release配置不同的输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${LIB_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${LIB_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${LIB_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${LIB_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RelWithDebInfo ${BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RelWithDebInfo ${LIB_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RelWithDebInfo ${LIB_DIR})



project(DragonRenderer 
    VERSION 1.0
    LANGUAGES CXX
)

include_directories(${CMAKE_SOURCE_DIR}/core)


# 递归拷贝assets目录
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/assets)
set(DEST_DIR ${BIN_DIR})
file(COPY ${SOURCE_DIR} DESTINATION ${DEST_DIR})

add_subdirectory(thirdparts)
add_subdirectory(application)
add_subdirectory(render)

add_executable(DragonRenderer "entry_point.cc")
target_link_libraries(DragonRenderer  Application Render thirdpart_lib_glm)


# TODO 可优化为接口库，为所有的可执行程序或者测试程序附加
target_link_options(DragonRenderer PRIVATE "/subsystem:console" "/entry:wWinMainCRTStartup")
# or set_target_properties(DragonRenderer PROPERTIES LINK_FLAGS "/subsystem:console /entry:wWinMainCRTStartup")

add_subdirectory(test)

