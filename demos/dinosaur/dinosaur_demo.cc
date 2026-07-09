#include "layer_registry.h"

#include "dinosaur_layer.h"

namespace
{
LayerAutoRegistrar g_dinosaur_layer_registrar([](Renderer* renderer) -> Layer* {
    return new DinosaurLayer(renderer);
});
}
