#include "layer_stack.h"

#include "layer.h"

LayerStack::~LayerStack()
{
    Clear();
}

void LayerStack::PushLayer(Layer* layer)
{
    if(layer == nullptr)
    {
        return;
    }

    layer->Init();
    layers_.push_back(layer);
}

void LayerStack::Clear()
{
    for(auto layer : layers_)
    {
        if(layer != nullptr)
        {
            layer->Destroy();
            delete layer;
        }
    }

    layers_.clear();
}

void LayerStack::Update()
{
    for(auto layer : layers_)
    {
        if(layer != nullptr)
        {
            layer->Update();
        }
    }
}

void LayerStack::Render()
{
    for(auto layer : layers_)
    {
        if(layer != nullptr)
        {
            layer->Render();
        }
    }
}
