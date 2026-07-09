add_library(DinosaurDemo STATIC
    dinosaur_demo.cc
    dinosaur_layer.cc
    dinosaur_layer.h
    ../demo_layer_factory.h
)

target_include_directories(DinosaurDemo PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(DinosaurDemo PUBLIC Render)
