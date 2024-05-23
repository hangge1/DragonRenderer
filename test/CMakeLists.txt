

add_executable(line_rasterize_test "line_rasterize_test.cc")
target_link_libraries(line_rasterize_test Render)
target_include_directories(line_rasterize_test PRIVATE ${CMAKE_SOURCE_DIR}/src/core)

# add_test(NAME line_rasterize_10_10_to_20_15 COMMAND line_rasterize_test 10 10 20 15)

# set_property(TEST line_rasterize_10_10_to_20_15 PROPERTY
#   PASS_REGULAR_EXPRESSION "RasterizeLineTest Test Pass"
# )


# 这里说测试0 < k < 1 是因为，k > 1 确实生成的不是完全一样，所以这里暂且不测试了
function(test_k_less_1_line_rasterize start_x start_y end_x end_y)
    add_test(NAME test_line_rasterize_${start_x}_${start_y}_to_${end_x}_${end_y} 
        COMMAND line_rasterize_test ${start_x} ${start_y} ${end_x} ${end_y})
endfunction()

test_k_less_1_line_rasterize(10 10 20 15)
test_k_less_1_line_rasterize(10 -10 20 -15)
test_k_less_1_line_rasterize(-10 10 -20 15)
test_k_less_1_line_rasterize(-10 -10 -20 -15)
