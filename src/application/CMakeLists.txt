# src/application/CMakeLists.txt

file(GLOB_RECURSE SRCS . "*.cc")
file(GLOB_RECURSE INCLUDES . "*.h")

add_library(Application ${SRCS} ${INCLUDES})

target_include_directories(Application PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(Application PRIVATE ${CMAKE_SOURCE_DIR}/core)