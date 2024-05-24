# thirdparts/CMakeLists.txt

#============================================================================
# 给glm库套一层，封装成接口库，方便依赖着不需要设置include_directory属性
add_subdirectory(glm)

add_library(thirdpart_lib_glm INTERFACE)
target_link_libraries(thirdpart_lib_glm INTERFACE glm)
target_include_directories(thirdpart_lib_glm INTERFACE ${CMAKE_SOURCE_DIR}/thirdparts)

#============================================================================
