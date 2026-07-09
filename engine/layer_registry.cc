#include "layer_registry.h"

#include "renderer.h"

LayerRegistry& LayerRegistry::Get()
{
    static LayerRegistry registry;
    return registry;
}

void LayerRegistry::RegisterLayerFactory(LayerFactory factory)
{
    if(factory)
    {
        factories_.push_back(factory);
    }
}

std::vector<Layer*> LayerRegistry::CreateLayers(Renderer* renderer) const
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

bool LayerRegistry::Empty() const
{
    return factories_.empty();
}

LayerAutoRegistrar::LayerAutoRegistrar(LayerRegistry::LayerFactory factory)
{
    LayerRegistry::Get().RegisterLayerFactory(factory);
}

void AttachRegisteredLayers(Renderer* renderer)
{
    if(renderer == nullptr)
    {
        return;
    }

    for(Layer* layer : LayerRegistry::Get().CreateLayers(renderer))
    {
        renderer->AddLayer(layer);
    }
}
