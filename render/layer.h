#ifndef _RENDER_LAYER_H_
#define _RENDER_LAYER_H_

#include <string>

class Layer
{
public:
    Layer() = default;
    virtual ~Layer() = default;

    virtual void Init() = 0;
    virtual void Destroy() = 0;

    virtual void Update() = 0;
    virtual void Render() = 0;

    virtual const char* Name() = 0;

protected:
    std::string name_;
};

#endif