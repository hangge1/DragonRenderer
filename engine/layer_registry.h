#ifndef _ENGINE_LAYER_REGISTRY_H_
#define _ENGINE_LAYER_REGISTRY_H_

#include <functional>
#include <vector>

class Layer;
class Renderer;

class LayerRegistry
{
public:
    using LayerFactory = std::function<Layer*(Renderer*)>;

    static LayerRegistry& Get();

    void RegisterLayerFactory(LayerFactory factory);
    std::vector<Layer*> CreateLayers(Renderer* renderer) const;
    bool Empty() const;

private:
    std::vector<LayerFactory> factories_;
};

class LayerAutoRegistrar
{
public:
    explicit LayerAutoRegistrar(LayerRegistry::LayerFactory factory);
};

void AttachRegisteredLayers(Renderer* renderer);

#endif
