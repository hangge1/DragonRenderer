#include "demo_layer_factory.h"

#include "dinosaur_layer.h"

Layer* CreateDemoLayer(Renderer* renderer)
{
    return new DinosaurLayer(renderer);
}
