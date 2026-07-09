#include "application_config.h"
#include "layer_registry.h"

#include "dinosaur_layer.h"

namespace
{
ApplicationConfigAutoRegistrar g_dinosaur_application_config([](ApplicationConfig& config) {
    WindowConfig& window = config.AddWindow(L"Dragon Soft Renderer - Dinosaur", 1200, 900);
    window.enable_interactive_resolution_scale = true;
    window.interactive_render_scale = 0.5f;
    window.interactive_recovery_ms = 180;
});

LayerAutoRegistrar g_dinosaur_layer_registrar([](Renderer* renderer) -> Layer* {
    return new DinosaurLayer(renderer);
});
}
