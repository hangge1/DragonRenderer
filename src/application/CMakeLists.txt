# src/application/CMakeLists.txt

file(GLOB_RECURSE SRCS . "*.cc")
file(GLOB_RECURSE INCLUDES . "*.h")

# 设置库的路径
set(ASSIMP_LIB_PATH ${CMAKE_SOURCE_DIR}/thirdparts/assimp/${CMAKE_BUILD_TYPE}/lib)
set(ASSIMP_BIN_PATH ${CMAKE_SOURCE_DIR}/thirdparts/assimp/${CMAKE_BUILD_TYPE}/bin)
set(ASSIMP_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/thirdparts/assimp/include)

# 添加库的头文件路径
include_directories(${ASSIMP_INCLUDE_PATH})
# 添加库文件路径
link_directories(${ASSIMP_LIB_PATH})


add_library(Application ${SRCS} ${INCLUDES})

target_include_directories(Application PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(Application PRIVATE ${CMAKE_SOURCE_DIR}/src/core ${CMAKE_SOURCE_DIR}/src/render)

target_link_libraries(Application thirdpart_lib_glm)



if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(Application "${ASSIMP_LIB_PATH}/assimp-vc143-mtd.lib" assets_lib)
else()
    target_link_libraries(Application "${ASSIMP_LIB_PATH}/assimp-vc143-mt.lib" assets_lib)
endif()