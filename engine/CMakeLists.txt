add_library(DragonEngine OBJECT
    entry_point.cc
    app/application.cc
    app/application.h
    demo_layer_factory.h
)

target_include_directories(DragonEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/app
)

target_compile_definitions(DragonEngine PUBLIC UNICODE)

target_link_libraries(DragonEngine PUBLIC Render)
