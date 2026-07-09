#ifndef _DEMOS_DINOSAUR_DINOSAUR_LAYER_H_
#define _DEMOS_DINOSAUR_DINOSAUR_LAYER_H_

#include "layer.h"


class LambertLightShader;
class Model;
class Renderer;

class DinosaurLayer : public Layer
{
public:
    DinosaurLayer(Renderer* renderer)
        : renderer_(renderer), lightShader_(nullptr), model(nullptr)
    {
        name_ = Name();
    }
    virtual ~DinosaurLayer() = default;

    virtual void Init() override;
    virtual void Destroy() override;

    virtual void Update() override;
    virtual void Render() override;

    virtual const char* Name() override { return "DinosaurLayer"; }

protected:
    virtual void InitShader();
    virtual void InitModel();



private:
    Renderer* renderer_;
    LambertLightShader* lightShader_;
    Model* model;
};

#endif
