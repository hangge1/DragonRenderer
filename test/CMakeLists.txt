# test/CMakeLists.txt

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
set_tests_properties(test_thirdpart_lib_stb_image PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
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
set_tests_properties(thirdpart_lib_assimp PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
# =======================================================================================

# =======================================================================================
# render_output_smoke_test.cc

add_executable(Test_RenderOutputSmoke "render_output_smoke_test.cc")
target_link_libraries(Test_RenderOutputSmoke PRIVATE Render)

add_test(NAME render_output_smoke COMMAND Test_RenderOutputSmoke)
set_tests_properties(render_output_smoke PROPERTIES PASS_REGULAR_EXPRESSION "render output smoke Pass")
# =======================================================================================

# =======================================================================================
# clip_tool_test.cc

add_executable(Test_ClipTool "clip_tool_test.cc")
target_link_libraries(Test_ClipTool PRIVATE Render)

add_test(NAME clip_tool COMMAND Test_ClipTool)
set_tests_properties(clip_tool PROPERTIES PASS_REGULAR_EXPRESSION "clip tool test Pass")
# =======================================================================================

# =======================================================================================
# depth_output_smoke_test.cc

add_executable(Test_DepthOutputSmoke "depth_output_smoke_test.cc")
target_link_libraries(Test_DepthOutputSmoke PRIVATE Render)

add_test(NAME depth_output_smoke COMMAND Test_DepthOutputSmoke)
set_tests_properties(depth_output_smoke PROPERTIES PASS_REGULAR_EXPRESSION "depth output smoke Pass")
# =======================================================================================

# =======================================================================================
# ndc_perspective_smoke_test.cc

add_executable(Test_NdcPerspectiveSmoke "ndc_perspective_smoke_test.cc")
target_link_libraries(Test_NdcPerspectiveSmoke PRIVATE Render)

add_test(NAME ndc_perspective_smoke COMMAND Test_NdcPerspectiveSmoke)
set_tests_properties(ndc_perspective_smoke PROPERTIES PASS_REGULAR_EXPRESSION "ndc perspective smoke Pass")
# =======================================================================================

# =======================================================================================
# draw_command_validation_test.cc

add_executable(Test_DrawCommandValidation "draw_command_validation_test.cc")
target_link_libraries(Test_DrawCommandValidation PRIVATE Render)

add_test(NAME draw_command_validation COMMAND Test_DrawCommandValidation)
set_tests_properties(draw_command_validation PROPERTIES PASS_REGULAR_EXPRESSION "draw command validation Pass")
# =======================================================================================

# =======================================================================================
# input_state_test.cc

add_executable(Test_InputState "input_state_test.cc")
target_link_libraries(Test_InputState PRIVATE Render)

add_test(NAME input_state COMMAND Test_InputState)
set_tests_properties(input_state PROPERTIES PASS_REGULAR_EXPRESSION "input state test Pass")
# =======================================================================================

# =======================================================================================
# raster_viewport_clamp_test.cc

add_executable(Test_RasterViewportClamp "raster_viewport_clamp_test.cc")
target_link_libraries(Test_RasterViewportClamp PRIVATE Render)

add_test(NAME raster_viewport_clamp COMMAND Test_RasterViewportClamp)
set_tests_properties(raster_viewport_clamp PROPERTIES PASS_REGULAR_EXPRESSION "raster viewport clamp test Pass")
# =======================================================================================
