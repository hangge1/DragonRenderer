#include "application_config.h"

namespace
{
const WindowConfig& DefaultWindowConfig()
{
    static const WindowConfig window;
    return window;
}
}

WindowConfig& ApplicationConfig::AddWindow(const std::wstring& title, int32_t width, int32_t height)
{
    WindowConfig window;
    window.title = title;
    window.width = width;
    window.height = height;
    windows_.push_back(window);
    return windows_.back();
}

const WindowConfig& ApplicationConfig::GetPrimaryWindow() const
{
    if(windows_.empty())
    {
        return DefaultWindowConfig();
    }

    return windows_.front();
}

ApplicationConfigRegistry& ApplicationConfigRegistry::Get()
{
    static ApplicationConfigRegistry registry;
    return registry;
}

void ApplicationConfigRegistry::RegisterConfigurator(Configurator configurator)
{
    if(configurator)
    {
        configurators_.push_back(configurator);
    }
}

ApplicationConfig ApplicationConfigRegistry::BuildConfig() const
{
    ApplicationConfig config;

    for(const Configurator& configurator : configurators_)
    {
        configurator(config);
    }

    if(config.GetWindows().empty())
    {
        config.AddWindow(L"DragonRenderer", 800, 600);
    }

    return config;
}

ApplicationConfigAutoRegistrar::ApplicationConfigAutoRegistrar(ApplicationConfigRegistry::Configurator configurator)
{
    ApplicationConfigRegistry::Get().RegisterConfigurator(configurator);
}
