#ifndef _ENGINE_APP_APPLICATION_H_
#define _ENGINE_APP_APPLICATION_H_

#include <cstdint>

class Renderer;

struct ApplicationRunOptions
{
    uint32_t max_frames { 0 };
    bool print_summary { false };
};

class Application
{
public:
    virtual ~Application() = default;

    virtual bool Init(void* platform_instance) = 0;
    virtual void Run(const ApplicationRunOptions& options = {}) = 0;
    virtual int32_t GetWidth() const = 0;
    virtual int32_t GetHeight() const = 0;
    virtual Renderer* GetRenderer() const = 0;
    virtual void RequestExit() = 0;
};

Application* CreateApplication();

#endif
