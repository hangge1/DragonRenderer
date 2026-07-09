add_library(DinosaurDemo STATIC
    dinosaur_demo.cc
    dinosaur_layer.cc
    dinosaur_layer.h
    ${CMAKE_SOURCE_DIR}/engine/demo_layer_registry.h
)

target_include_directories(DinosaurDemo PUBLIC
    ${CMAKE_SOURCE_DIR}/engine
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(DinosaurDemo PUBLIC Render)
