# src/CMakeLists.txt

cmake_minimum_required(VERSION 3.20)

project(DragonRenderer)

add_executable(app "main.cpp" "application/application.cc")

target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/application)

