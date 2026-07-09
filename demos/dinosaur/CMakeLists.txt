add_library(DinosaurDemo STATIC
    dinosaur_layer.cc
    dinosaur_layer.h
)

target_include_directories(DinosaurDemo PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(DinosaurDemo PUBLIC Render)
