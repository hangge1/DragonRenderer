#include "demo_layer_registry.h"

void DemoLayerRegistry::RegisterLayerFactory(LayerFactory factory)
{
    if(factory)
    {
        factories_.push_back(factory);
    }
}

std::vector<Layer*> DemoLayerRegistry::CreateLayers(Renderer* renderer) const
{
    std::vector<Layer*> layers;
    layers.reserve(factories_.size());

    for(const LayerFactory& factory : factories_)
    {
        Layer* layer = factory(renderer);
        if(layer != nullptr)
        {
            layers.push_back(layer);
        }
    }

    return layers;
}

bool DemoLayerRegistry::Empty() const
{
    return factories_.empty();
}
