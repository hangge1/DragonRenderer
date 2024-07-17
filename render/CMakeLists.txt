# src/render/CMakeLists.txt

file(GLOB_RECURSE SRCS . "*.cc")
file(GLOB_RECURSE INCLUDES . "*.h")

add_library(Render STATIC ${SRCS} ${INCLUDES})

target_include_directories(Render PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# 宽字节字符集
target_compile_definitions(Render PUBLIC UNICODE)

# link lib glm
target_link_libraries(Render PUBLIC thirdpart_lib_stb_image thirdpart_lib_glm)

# -----------------------------assimp------------------------------------
# 设置库的路径
set(ASSIMP_LIB_PATH ${CMAKE_SOURCE_DIR}/thirdparts/assimp/${CMAKE_BUILD_TYPE}/lib)
set(ASSIMP_BIN_PATH ${CMAKE_SOURCE_DIR}/thirdparts/assimp/${CMAKE_BUILD_TYPE}/bin)
set(ASSIMP_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/thirdparts/assimp/include)

message("======================ASSIMP_INCLUDE_PATH = ${ASSIMP_INCLUDE_PATH}=======================")

# 添加库的头文件路径
target_include_directories(Render PUBLIC ${ASSIMP_INCLUDE_PATH})
# 添加库文件路径
target_link_directories(Render PUBLIC ${ASSIMP_LIB_PATH})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(Render PRIVATE "${ASSIMP_LIB_PATH}/assimp-vc143-mtd.lib")
else()
    target_link_libraries(Render PRIVATE "${ASSIMP_LIB_PATH}/assimp-vc143-mt.lib")
endif()