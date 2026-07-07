#ifndef _RENDER_LAYER_STACK_H_
#define _RENDER_LAYER_STACK_H_

#include <vector>

class Layer;

class LayerStack
{
public:
    LayerStack() = default;
    ~LayerStack();

    LayerStack(const LayerStack&) = delete;
    LayerStack(LayerStack&&) = delete;
    LayerStack& operator=(const LayerStack&) = delete;
    LayerStack& operator=(LayerStack&&) = delete;

    void PushLayer(Layer* layer);
    void Clear();

    void Update();
    void Render();

private:
    std::vector<Layer*> layers_;
};

#endif
