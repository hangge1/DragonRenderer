#include "application_config.h"
#include "layer_registry.h"

#include "dinosaur_layer.h"

namespace
{
ApplicationConfigAutoRegistrar g_dinosaur_application_config([](ApplicationConfig& config) {
    WindowConfig& window = config.AddWindow(L"Dragon Soft Renderer - Dinosaur", 1200, 900);
    window.enable_interactive_resolution_scale = true;
    window.interactive_render_scale = 0.45f;
    window.interactive_recovery_ms = 480;
    window.interactive_recovery_steps = 4;
    window.interactive_target_render_ms = 8.0;
    window.interactive_target_coverage = 0.16f;
    window.interactive_scale_step = 0.1f;
});

LayerAutoRegistrar g_dinosaur_layer_registrar([](Renderer* renderer) -> Layer* {
    return new DinosaurLayer(renderer);
});
}
