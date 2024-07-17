#ifndef _RENDER_LAYER_TEST_LAYER_H_
#define _RENDER_LAYER_TEST_LAYER_H_

#include "layer.h"


class LambertLightShader;
class Model;
class Renderer;

class TestLayer : public Layer
{
public:
    TestLayer(Renderer* renderer)
        : renderer_(renderer)
    {
        name_ = Name();
    }
    virtual ~TestLayer() = default;

    virtual void Init() override;
    virtual void Destroy() override;

    virtual void Update() override;
    virtual void Render() override;

    virtual const char* Name() override { return "TestLayer"; }

private:
    Renderer* renderer_;
    LambertLightShader* lightShader_ { nullptr };
    Model* model { nullptr };
};

#endif