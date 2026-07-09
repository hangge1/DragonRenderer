#include "windows_application.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <windowsx.h>

#include "event.h"
#include "layer_registry.h"
#include "renderer.h"
#include "runtime/scoped_timer.h"

namespace
{
WindowsApplication* g_active_application = nullptr;

LRESULT CALLBACK Wndproc(HWND window_handler, UINT message_id, WPARAM message_wparam, LPARAM message_lparam)
{
    if(g_active_application != nullptr)
    {
        g_active_application->ProcessMessage(window_handler, message_id, message_wparam, message_lparam);
    }

    return DefWindowProc(window_handler, message_id, message_wparam, message_lparam);
}
}

WindowsApplication::~WindowsApplication()
{
    if(nullptr != bitmap_)
    {
        DeleteObject(bitmap_);
    }

    if(nullptr != canvasDC_)
    {
        DeleteDC(canvasDC_);
    }

    if(nullptr != currentDC_ && nullptr != hwnd_)
    {
        ReleaseDC(hwnd_, currentDC_);
    }

    if(g_active_application == this)
    {
        g_active_application = nullptr;
    }
}

bool WindowsApplication::Init(void* platform_instance, const wchar_t* window_title,
    int32_t window_width, int32_t window_height)
{
    width_ = window_width;
    height_ = window_height;
    hinstance_ = static_cast<HINSTANCE>(platform_instance);
    title_ = window_title;
    g_active_application = this;

    RegisterMainWindowClass();

    if(!CreateMainWindow())
    {
        return false;
    }

    InitDC();

    renderer_ = new Renderer();
    renderer_->SetExitRequestedCallback([this]() { RequestExit(); });
    renderer_->Init(width_, height_, canvas_buffer_);
    AttachRegisteredLayers(renderer_);

    return true;
}

void WindowsApplication::Run(const ApplicationRunOptions& options)
{
    using Clock = std::chrono::steady_clock;

    auto fps_sample_start = Clock::now();
    uint32_t sample_frame_count = 0;
    uint32_t total_frame_count = 0;

    double total_frame_ms = 0.0;
    double total_update_ms = 0.0;
    double total_render_ms = 0.0;
    double total_present_ms = 0.0;
    double total_vertex_stage_ms = 0.0;
    double total_clip_stage_ms = 0.0;
    double total_ndc_stage_ms = 0.0;
    double total_cull_stage_ms = 0.0;
    double total_viewport_stage_ms = 0.0;
    double total_raster_stage_ms = 0.0;
    double total_fragment_output_stage_ms = 0.0;
    uint64_t total_draw_calls = 0;
    uint64_t total_input_triangles = 0;
    uint64_t total_rasterized_fragments = 0;

    while(IsInLoop())
    {
        renderer_->BeginFrameStats();

        double frame_ms = 0.0;
        double update_ms = 0.0;
        double render_ms = 0.0;
        double present_ms = 0.0;

        {
            ScopedTimer frame_timer(frame_ms);

            renderer_->Clear();

            {
                ScopedTimer update_timer(update_ms);
                renderer_->OnUpdate();
            }

            {
                ScopedTimer render_timer(render_ms);
                renderer_->Render();
            }

            {
                ScopedTimer present_timer(present_ms);
                SwapBuffer();
            }
        }

        renderer_->SetFrameTiming(frame_ms, update_ms, render_ms, present_ms);
        const auto& stats = renderer_->GetFrameStats();

        total_frame_count++;
        total_frame_ms += stats.frame_ms;
        total_update_ms += stats.update_ms;
        total_render_ms += stats.render_ms;
        total_present_ms += stats.present_ms;
        total_vertex_stage_ms += stats.vertex_stage_ms;
        total_clip_stage_ms += stats.clip_stage_ms;
        total_ndc_stage_ms += stats.ndc_stage_ms;
        total_cull_stage_ms += stats.cull_stage_ms;
        total_viewport_stage_ms += stats.viewport_stage_ms;
        total_raster_stage_ms += stats.raster_stage_ms;
        total_fragment_output_stage_ms += stats.fragment_output_stage_ms;
        total_draw_calls += stats.draw_calls;
        total_input_triangles += stats.input_triangles;
        total_rasterized_fragments += stats.rasterized_fragments;

        sample_frame_count++;
        auto now = Clock::now();
        double elapsed = std::chrono::duration<double>(now - fps_sample_start).count();
        if(elapsed >= 1.0)
        {
            double fps = sample_frame_count / elapsed;

            std::wostringstream title_stream;
            title_stream << (title_ != nullptr ? title_ : L"DragonRenderer")
                << L" | FPS: " << std::fixed << std::setprecision(1) << fps
                << L" | Frame: " << std::fixed << std::setprecision(2) << stats.frame_ms << L" ms"
                << L" | U/R/P: " << std::fixed << std::setprecision(2)
                << stats.update_ms << L"/" << stats.render_ms << L"/" << stats.present_ms << L" ms"
                << L" | Pipe: " << std::fixed << std::setprecision(2)
                << stats.vertex_stage_ms << L"/" << stats.clip_stage_ms << L"/"
                << stats.raster_stage_ms << L"/" << stats.fragment_output_stage_ms << L" ms"
                << L" | DC: " << stats.draw_calls
                << L" | Tri: " << stats.input_triangles
                << L" | Frag: " << stats.rasterized_fragments;
            SetWindowText(hwnd_, title_stream.str().c_str());

            sample_frame_count = 0;
            fps_sample_start = now;
        }

        if(options.max_frames > 0 && total_frame_count >= options.max_frames)
        {
            RequestExit();
        }
    }

    if(options.print_summary && total_frame_count > 0)
    {
        double count = static_cast<double>(total_frame_count);
        std::cout << "DragonRenderer benchmark summary" << std::endl;
        std::cout << "Frames: " << total_frame_count << std::endl;
        std::cout << "Average frame: " << total_frame_ms / count << " ms" << std::endl;
        std::cout << "Average update/render/present: "
            << total_update_ms / count << " / "
            << total_render_ms / count << " / "
            << total_present_ms / count << " ms" << std::endl;
        std::cout << "Average pipeline vertex/clip/ndc/cull/viewport/raster/fragment-output: "
            << total_vertex_stage_ms / count << " / "
            << total_clip_stage_ms / count << " / "
            << total_ndc_stage_ms / count << " / "
            << total_cull_stage_ms / count << " / "
            << total_viewport_stage_ms / count << " / "
            << total_raster_stage_ms / count << " / "
            << total_fragment_output_stage_ms / count << " ms" << std::endl;
        std::cout << "Average draw calls: " << total_draw_calls / count << std::endl;
        std::cout << "Average input triangles: " << total_input_triangles / count << std::endl;
        std::cout << "Average rasterized fragments: " << total_rasterized_fragments / count << std::endl;
    }

    delete renderer_;
    renderer_ = nullptr;
}

void WindowsApplication::ProcessMessage(HWND window_handler, UINT message_id, WPARAM message_wparam, LPARAM message_lparam)
{
    switch(message_id)
    {
        case WM_KEYDOWN:
        {
            if(renderer_ == nullptr)
            {
                break;
            }
            KeyDownEvent ev(message_wparam);
            renderer_->OnEvent(ev);
        }
        break;
        case WM_KEYUP:
        {
            if(renderer_ == nullptr)
            {
                break;
            }
            KeyUpEvent ev(message_wparam);
            renderer_->OnEvent(ev);
        }
        break;
        case WM_RBUTTONDOWN:
        {
            if(renderer_ == nullptr)
            {
                break;
            }
            MouseDownEvent ev(GET_X_LPARAM(message_lparam), GET_Y_LPARAM(message_lparam), 2);
            renderer_->OnEvent(ev);
        }
        break;
        case WM_RBUTTONUP:
        {
            if(renderer_ == nullptr)
            {
                break;
            }
            MouseUpEvent ev(GET_X_LPARAM(message_lparam), GET_Y_LPARAM(message_lparam), 2);
            renderer_->OnEvent(ev);
        }
        break;
        case WM_MOUSEMOVE:
        {
            if(renderer_ == nullptr)
            {
                break;
            }
            MouseMoveEvent ev(GET_X_LPARAM(message_lparam), GET_Y_LPARAM(message_lparam));
            renderer_->OnEvent(ev);
        }
        break;
        case WM_CLOSE:
        {
            DestroyWindow(window_handler);
        }
        break;
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            BeginPaint(window_handler, &ps);
            EndPaint(window_handler, &ps);
        }
        break;
        case WM_DESTROY:
        {
            has_destoryed_ = true;
            PostQuitMessage(0);
        }
        break;
    }
}

bool WindowsApplication::IsInLoop()
{
    if(has_destoryed_)
    {
        return false;
    }

    MSG msg;
    if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return true;
}

void WindowsApplication::SwapBuffer()
{
    BitBlt(currentDC_, 0, 0, width_, height_, canvasDC_, 0, 0, SRCCOPY);
}

bool WindowsApplication::CreateMainWindow()
{
    auto dwExStyle = WS_EX_APPWINDOW;
    auto dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    RECT windowRect;
    windowRect.left = 0L;
    windowRect.top = 0L;
    windowRect.right = (LONG)width_;
    windowRect.bottom = (LONG)height_;
    AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

    hwnd_ = CreateWindow(
        register_class_name_,
        title_,
        dwStyle,
        500,
        500,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,
        nullptr,
        hinstance_,
        nullptr);

    if(!hwnd_)
    {
        return false;
    }

    MoveWindow2DesktopCenter();

    ShowWindow(hwnd_, true);
    UpdateWindow(hwnd_);

    return true;
}

void WindowsApplication::MoveWindow2DesktopCenter()
{
    RECT rectWindow;
    GetWindowRect(hwnd_, &rectWindow);

    RECT rectScreen;
    GetClientRect(GetDesktopWindow(), &rectScreen);

    int posX = (rectScreen.right - rectWindow.right + rectWindow.left) / 2;
    int posY = (rectScreen.bottom - rectWindow.bottom + rectWindow.top) / 2;

    SetWindowPos(hwnd_, HWND_TOP, posX, posY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

ATOM WindowsApplication::RegisterMainWindowClass()
{
    WNDCLASSEX wndClass;

    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = Wndproc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hinstance_;
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = register_class_name_;
    wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

    return RegisterClassEx(&wndClass);
}

void WindowsApplication::InitDC()
{
    currentDC_ = GetDC(hwnd_);
    canvasDC_ = CreateCompatibleDC(currentDC_);

    BITMAPINFO temp_bmp_info {};
    temp_bmp_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    temp_bmp_info.bmiHeader.biWidth = width_;
    temp_bmp_info.bmiHeader.biHeight = height_;
    temp_bmp_info.bmiHeader.biPlanes = 1;
    temp_bmp_info.bmiHeader.biBitCount = 32;
    temp_bmp_info.bmiHeader.biCompression = BI_RGB;

    bitmap_ = CreateDIBSection(canvasDC_, &temp_bmp_info, DIB_RGB_COLORS, (void**)&canvas_buffer_, 0, 0);

    SelectObject(canvasDC_, bitmap_);

    memset(canvas_buffer_, 0, width_ * height_ * 4);
}

Application* CreateApplication()
{
    return new WindowsApplication();
}
