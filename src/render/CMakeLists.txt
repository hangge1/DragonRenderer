# src/render/CMakeLists.txt

file(GLOB_RECURSE SRCS . "*.cc")
file(GLOB_RECURSE INCLUDES . "*.h")

add_library(Render ${SRCS} ${INCLUDES})

target_include_directories(Render PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(Render PRIVATE ${CMAKE_SOURCE_DIR}/src/core)

# use lib glm
target_include_directories(Render PRIVATE ${CMAKE_SOURCE_DIR}/thirdparts)
target_link_libraries(Render glm)