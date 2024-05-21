# src/CMakeLists.txt

add_subdirectory(application)
add_subdirectory(render)

add_executable(DragonRenderer "main.cpp")
target_link_libraries(DragonRenderer  Application Render)
target_include_directories(DragonRenderer PRIVATE ${CMAKE_SOURCE_DIR}/src/core)



