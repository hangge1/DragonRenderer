add_library(DragonEngine OBJECT
    entry_point.cc
    app/application.h
    demo_layer_registry.cc
    demo_layer_registry.h
    platform/win32/windows_application.cc
    platform/win32/windows_application.h
)

target_include_directories(DragonEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/app
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/win32
)

target_compile_definitions(DragonEngine PUBLIC UNICODE)

target_link_libraries(DragonEngine PUBLIC Render)
