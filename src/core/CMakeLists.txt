
file(GLOB_RECURSE SRCS . "*.cc")
file(GLOB_RECURSE INCLUDES . "*.h")

add_library(CoreLib ${SRCS} ${INCLUDES})

target_include_directories(CoreLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(CoreLib thirdpart_lib_stb_image thirdpart_lib_glm)