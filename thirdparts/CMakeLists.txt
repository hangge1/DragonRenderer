# thirdparts/CMakeLists.txt

#============================================================================
# 给glm库套一层，封装成接口库，方便依赖着不需要设置include_directory属性
add_subdirectory(glm)

add_library(thirdpart_lib_glm INTERFACE)
target_link_libraries(thirdpart_lib_glm INTERFACE glm)
target_include_directories(thirdpart_lib_glm INTERFACE ${CMAKE_SOURCE_DIR}/thirdparts)

#============================================================================

#============================================================================
# stb_image 图片解码库
add_library(thirdpart_lib_stb_image INTERFACE)
target_include_directories(thirdpart_lib_stb_image INTERFACE ${CMAKE_SOURCE_DIR}/thirdparts/stb_image)
target_compile_definitions(thirdpart_lib_stb_image INTERFACE STB_IMAGE_IMPLEMENTATION)

#============================================================================

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
else()
    file(COPY ${ASSIMP_BIN_PATH}/assimp-vc143-mt.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})
endif()

#============================================================================