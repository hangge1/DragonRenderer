# src/render/CMakeLists.txt

file(GLOB_RECURSE SRCS . "*.cc")
file(GLOB_RECURSE INCLUDES . "*.h")

add_library(Render ${SRCS} ${INCLUDES})

target_include_directories(Render PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# link lib glm
target_link_libraries(Render thirdpart_lib_glm PipelineDataLib)