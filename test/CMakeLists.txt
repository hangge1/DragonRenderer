# test/CMakeLists.txt

# =======================老式的接口的测试，目前已经移除，所以不需要了===========================================
# line_rasterize_test.cc

# add_executable(Test_LineRasterize "line_rasterize_test.cc")
# target_link_libraries(Test_LineRasterize Render)
# target_include_directories(Test_LineRasterize PRIVATE ${CMAKE_SOURCE_DIR}/core)


# # 这里说测试0 < k < 1 是因为，k > 1 确实生成的不是完全一样，所以这里暂且不测试了
# function(test_k_less_1_line_rasterize start_x start_y end_x end_y)
#     set(test_name "test_line_rasterize_${start_x}_${start_y}_to_${end_x}_${end_y}")
#     add_test(NAME ${test_name} COMMAND Test_LineRasterize ${start_x} ${start_y} ${end_x} ${end_y})
#     set_tests_properties(${test_name} PROPERTIES PASS_REGULAR_EXPRESSION "line_rasterize_test Test Pass")
# endfunction()

# test_k_less_1_line_rasterize(10 10 20 15)
# test_k_less_1_line_rasterize(10 -10 20 -15)
# test_k_less_1_line_rasterize(-10 10 -20 15)
# test_k_less_1_line_rasterize(-10 -10 -20 -15)
# =======================================================================================

# =======================================================================================
# glm_test.cc

add_executable(Test_GLM "glm_test.cc")
target_link_libraries(Test_GLM PRIVATE thirdpart_lib_glm)

add_test(NAME test_thirdpart_lib_glm COMMAND Test_GLM) 
set_tests_properties(test_thirdpart_lib_glm PROPERTIES PASS_REGULAR_EXPRESSION "thirdparts library glm_test Pass")
# =======================================================================================

# =======================================================================================
# stb_image_test.cc

add_executable(Test_StbImage "stb_image_test.cc")
target_link_libraries(Test_StbImage PRIVATE thirdpart_lib_stb_image)

add_test(NAME test_thirdpart_lib_stb_image COMMAND Test_StbImage) 
set_tests_properties(test_thirdpart_lib_stb_image PROPERTIES PASS_REGULAR_EXPRESSION "thirdparts library stb_image Pass")
# =======================================================================================


# =======================================================================================
# assimp_test.cc

add_executable(Test_Assimp "assimp_test.cc")

# -----------------------------assimp------------------------------------
# 设置库的路径
set(ASSIMP_LIB_PATH ${CMAKE_SOURCE_DIR}/thirdparts/assimp/${CMAKE_BUILD_TYPE}/lib)
set(ASSIMP_BIN_PATH ${CMAKE_SOURCE_DIR}/thirdparts/assimp/${CMAKE_BUILD_TYPE}/bin)
set(ASSIMP_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/thirdparts/assimp/include)

# 添加库的头文件路径
target_include_directories(Test_Assimp PUBLIC ${ASSIMP_INCLUDE_PATH})
# 添加库文件路径
target_link_directories(Test_Assimp PUBLIC ${ASSIMP_LIB_PATH})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(Test_Assimp PRIVATE "${ASSIMP_LIB_PATH}/assimp-vc143-mtd.lib")
else()
    target_link_libraries(Test_Assimp PRIVATE "${ASSIMP_LIB_PATH}/assimp-vc143-mt.lib")
endif()

add_test(NAME thirdpart_lib_assimp COMMAND Test_Assimp) 
set_tests_properties(thirdpart_lib_assimp PROPERTIES PASS_REGULAR_EXPRESSION "thirdparts library assimp Pass")
# =======================================================================================