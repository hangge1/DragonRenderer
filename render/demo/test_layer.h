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
        : renderer_(renderer), lightShader_(nullptr), model(nullptr)
    {
        name_ = Name();
    }
    virtual ~TestLayer() = default;

    virtual void Init() override;
    virtual void Destroy() override;

    virtual void Update() override;
    virtual void Render() override;

    virtual const char* Name() override { return "TestLayer"; }

protected:
    virtual void InitShader();
    virtual void InitModel();



private:
    Renderer* renderer_;
    LambertLightShader* lightShader_;
    Model* model;
};

#endif