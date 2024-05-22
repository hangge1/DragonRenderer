

add_executable(line_rasterize_test "line_rasterize_test.cc")
target_link_libraries(line_rasterize_test Render)
target_include_directories(line_rasterize_test PRIVATE ${CMAKE_SOURCE_DIR}/src/core)