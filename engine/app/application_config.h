#ifndef _ENGINE_APP_APPLICATION_CONFIG_H_
#define _ENGINE_APP_APPLICATION_CONFIG_H_

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

struct WindowConfig
{
    std::wstring title { L"DragonRenderer" };
    int32_t width { 800 };
    int32_t height { 600 };
    bool visible { true };
    bool enable_interactive_resolution_scale { false };
    float interactive_render_scale { 0.5f };
    uint32_t interactive_recovery_ms { 480 };
    uint32_t interactive_recovery_steps { 4 };
    double interactive_target_render_ms { 8.0 };
    float interactive_target_coverage { 0.16f };
    float interactive_scale_step { 0.1f };
};

class ApplicationConfig
{
public:
    WindowConfig& AddWindow(const std::wstring& title, int32_t width, int32_t height);
    const std::vector<WindowConfig>& GetWindows() const { return windows_; }
    const WindowConfig& GetPrimaryWindow() const;

private:
    std::vector<WindowConfig> windows_;
};

class ApplicationConfigRegistry
{
public:
    using Configurator = std::function<void(ApplicationConfig&)>;

    static ApplicationConfigRegistry& Get();

    void RegisterConfigurator(Configurator configurator);
    ApplicationConfig BuildConfig() const;

private:
    std::vector<Configurator> configurators_;
};

class ApplicationConfigAutoRegistrar
{
public:
    explicit ApplicationConfigAutoRegistrar(ApplicationConfigRegistry::Configurator configurator);
};

#endif
