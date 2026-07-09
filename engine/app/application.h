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
    static constexpr int32_t kDefaultWindowWidth = 800;
    static constexpr int32_t kDefaultWindowHeight = 600;
    static constexpr const wchar_t* kDefaultWindowTitle = L"DragonRenderer";

    virtual ~Application() = default;

    virtual bool Init(void* platform_instance,
        const wchar_t* main_window_title = kDefaultWindowTitle,
        int32_t main_window_width = kDefaultWindowWidth,
        int32_t main_window_height = kDefaultWindowHeight) = 0;

    virtual void Run(const ApplicationRunOptions& options = {}) = 0;
    virtual int32_t GetWidth() const = 0;
    virtual int32_t GetHeight() const = 0;
    virtual Renderer* GetRenderer() const = 0;
    virtual void RequestExit() = 0;
};

Application* CreateApplication();

#endif
