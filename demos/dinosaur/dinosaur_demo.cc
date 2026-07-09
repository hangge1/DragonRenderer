#include "application_config.h"
#include "layer_registry.h"

#include "dinosaur_layer.h"

namespace
{
ApplicationConfigAutoRegistrar g_dinosaur_application_config([](ApplicationConfig& config) {
    config.AddWindow(L"Dragon Soft Renderer - Dinosaur", 1200, 900);
});

LayerAutoRegistrar g_dinosaur_layer_registrar([](Renderer* renderer) -> Layer* {
    return new DinosaurLayer(renderer);
});
}
