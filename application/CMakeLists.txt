# src/application/CMakeLists.txt

file(GLOB_RECURSE SRCS . "*.cc")
file(GLOB_RECURSE INCLUDES . "*.h")


add_library(Application STATIC ${SRCS} ${INCLUDES})

target_include_directories(Application PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} PUBLIC ${CMAKE_SOURCE_DIR}/render)

target_link_libraries(Application PUBLIC thirdpart_lib_glm)

# 宽字节字符集
target_compile_definitions(Application PUBLIC UNICODE)

