/*
    Author: hangge1
    Description: Engine-owned executable entry point.
*/
#include <Windows.h>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include "application.h"
#include "demo_layer_registry.h"
#include "layer.h"
#include "renderer.h"

namespace
{
ApplicationRunOptions ParseRunOptions(const wchar_t* command_line)
{
    ApplicationRunOptions options;

    if(command_line == nullptr)
    {
        return options;
    }

    std::wstring args(command_line);
    auto parse_frame_count = [&args](const std::wstring& flag) -> uint32_t {
        auto flag_pos = args.find(flag);
        if(flag_pos == std::wstring::npos)
        {
            return 0;
        }

        auto value_pos = args.find_first_not_of(L" \t", flag_pos + flag.size());
        if(value_pos == std::wstring::npos)
        {
            return 0;
        }

        return static_cast<uint32_t>(std::wcstoul(args.c_str() + value_pos, nullptr, 10));
    };

    uint32_t benchmark_frames = parse_frame_count(L"--benchmark");
    uint32_t smoke_frames = parse_frame_count(L"--smoke");

    if(benchmark_frames > 0)
    {
        options.max_frames = benchmark_frames;
        options.print_summary = true;
    }
    else if(smoke_frames > 0)
    {
        options.max_frames = smoke_frames;
        options.print_summary = true;
    }

    return options;
}
}

int WINAPI wWinMain(HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPTSTR lpCmdLine,
                    int nCmdShow)
{
    const int window_width = 1200;
    const int window_height = 900;
    const wchar_t* window_title = L"DragonSoftRenderer";

    std::unique_ptr<Application> app(CreateApplication());
    if(!app || !app->Init(static_cast<void*>(hInstance), window_title, window_width, window_height))
    {
        std::cout << "Application Init Failed!" << std::endl;
        return -1;
    }

    Renderer* renderer = app->GetRenderer();
    DemoLayerRegistry registry;
    RegisterDemoLayers(registry);

    for(Layer* demo_layer : registry.CreateLayers(renderer))
    {
        renderer->AddLayer(demo_layer);
    }

    app->Run(ParseRunOptions(lpCmdLine));

    std::cout << "Application will exit!" << std::endl;

    return 0;
}
