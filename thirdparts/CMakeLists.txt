# thirdparts/CMakeLists.txt

#============================================================================
# glm 数学库（接口库）
add_subdirectory(glm)

#============================================================================
# stb_image 图片解码库
add_subdirectory(stb_image)

#============================================================================

# assimp 模型读取库

# 设置库的路径
set(ASSIMP_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/assimp/${CMAKE_BUILD_TYPE}/lib)
set(ASSIMP_BIN_PATH ${CMAKE_CURRENT_SOURCE_DIR}/assimp/${CMAKE_BUILD_TYPE}/bin)
set(ASSIMP_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/assimp/include)


# 添加库的头文件路径
include_directories(${ASSIMP_INCLUDE_PATH})

# 添加库文件路径
link_directories(${ASSIMP_LIB_PATH})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    file(COPY ${ASSIMP_BIN_PATH}/assimp-vc143-mtd.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG})
    file(COPY ${ASSIMP_LIB_PATH}/assimp-vc143-mtd.lib DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG})
else()
    file(COPY ${ASSIMP_BIN_PATH}/assimp-vc143-mt.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})
    file(COPY ${ASSIMP_LIB_PATH}/assimp-vc143-mt.lib DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE})
endif()

#============================================================================