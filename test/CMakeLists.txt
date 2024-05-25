# test/CMakeLists.txt

# =======================================================================================
# line_rasterize_test.cc

add_executable(line_rasterize_test "line_rasterize_test.cc")
target_link_libraries(line_rasterize_test Render)
target_include_directories(line_rasterize_test PRIVATE ${CMAKE_SOURCE_DIR}/src/core)


# 这里说测试0 < k < 1 是因为，k > 1 确实生成的不是完全一样，所以这里暂且不测试了
function(test_k_less_1_line_rasterize start_x start_y end_x end_y)
    set(test_name "test_line_rasterize_${start_x}_${start_y}_to_${end_x}_${end_y}")
    add_test(NAME ${test_name} COMMAND line_rasterize_test ${start_x} ${start_y} ${end_x} ${end_y})
    set_tests_properties(${test_name} PROPERTIES PASS_REGULAR_EXPRESSION "line_rasterize_test Test Pass")
endfunction()

test_k_less_1_line_rasterize(10 10 20 15)
test_k_less_1_line_rasterize(10 -10 20 -15)
test_k_less_1_line_rasterize(-10 10 -20 15)
test_k_less_1_line_rasterize(-10 -10 -20 -15)
# =======================================================================================

# =======================================================================================
# glm_test.cc

add_executable(glm_test "glm_test.cc")
target_link_libraries(glm_test thirdpart_lib_glm)

add_test(NAME test_thirdpart_lib_glm COMMAND glm_test) 
set_tests_properties(test_thirdpart_lib_glm PROPERTIES PASS_REGULAR_EXPRESSION "thirdparts library glm_test Pass")
# =======================================================================================

# =======================================================================================
# stb_image_test.cc

add_executable(stb_image_test "stb_image_test.cc")
target_link_libraries(stb_image_test thirdpart_lib_stb_image assets_lib)

add_test(NAME test_thirdpart_lib_stb_image COMMAND stb_image_test) 
set_tests_properties(test_thirdpart_lib_stb_image PROPERTIES PASS_REGULAR_EXPRESSION "thirdparts library stb_image Pass")
# =======================================================================================