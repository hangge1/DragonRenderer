#ifndef _ENGINE_DEMO_LAYER_REGISTRY_H_
#define _ENGINE_DEMO_LAYER_REGISTRY_H_

#include <functional>
#include <vector>

class Layer;
class Renderer;

class DemoLayerRegistry
{
public:
    using LayerFactory = std::function<Layer*(Renderer*)>;

    void RegisterLayerFactory(LayerFactory factory);
    std::vector<Layer*> CreateLayers(Renderer* renderer) const;
    bool Empty() const;

private:
    std::vector<LayerFactory> factories_;
};

void RegisterDemoLayers(DemoLayerRegistry& registry);

#endif
