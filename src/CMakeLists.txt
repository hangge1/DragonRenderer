# src/CMakeLists.txt

cmake_minimum_required(VERSION 3.20)

project(DragonRenderer 
    VERSION 1.0
    LANGUAGES CXX
)

add_subdirectory(application)
add_subdirectory(render)

add_executable(DragonRenderer "main.cpp")

target_link_libraries(DragonRenderer  Application Render)

target_include_directories(DragonRenderer PRIVATE ${CMAKE_SOURCE_DIR}/core)



