
#============================================================================
# stb_image 图片解码库

add_library(thirdpart_lib_stb_image STATIC stb_image.cc stb_image.h)
target_include_directories(thirdpart_lib_stb_image PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#============================================================================