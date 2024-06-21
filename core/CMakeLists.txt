
file(GLOB_RECURSE SRCS . "*.cc")
file(GLOB_RECURSE INCLUDES . "*.h")

add_library(PipelineDataLib ${SRCS} ${INCLUDES})

target_include_directories(PipelineDataLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(PipelineDataLib thirdpart_lib_stb_image thirdpart_lib_glm)