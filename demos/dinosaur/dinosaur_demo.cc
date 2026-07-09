#include "demo_layer_registry.h"

#include "dinosaur_layer.h"

void RegisterDemoLayers(DemoLayerRegistry& registry)
{
    registry.RegisterLayerFactory([](Renderer* renderer) -> Layer* {
        return new DinosaurLayer(renderer);
    });
}
