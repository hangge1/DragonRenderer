#include "windows_application.h"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <windowsx.h>

#include "application_config.h"
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
    if(nullptr != canvasDC_ && nullptr != previous_canvas_object_)
    {
        SelectObject(canvasDC_, previous_canvas_object_);
        previous_canvas_object_ = nullptr;
    }

    if(nullptr != bitmap_)
    {
        DeleteObject(bitmap_);
        bitmap_ = nullptr;
    }

    if(nullptr != canvasDC_)
    {
        DeleteDC(canvasDC_);
        canvasDC_ = nullptr;
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

bool WindowsApplication::Init(void* platform_instance)
{
    ApplicationConfig config = ApplicationConfigRegistry::Get().BuildConfig();
    const WindowConfig& primary_window = config.GetPrimaryWindow();

    window_config_ = primary_window;
    width_ = primary_window.width;
    height_ = primary_window.height;
    render_width_ = width_;
    render_height_ = height_;
    hinstance_ = static_cast<HINSTANCE>(platform_instance);
    title_ = primary_window.title;
    g_active_application = this;

    RegisterMainWindowClass();

    if(!CreateMainWindow())
    {
        return false;
    }

    InitDC();

    renderer_ = new Renderer();
    renderer_->SetExitRequestedCallback([this]() { RequestExit(); });
    renderer_->Init(render_width_, render_height_, canvas_buffer_);
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

    while(!has_destoryed_)
    {
        input_state_.BeginFrame();
        if(!ProcessPendingMessages())
        {
            break;
        }
        if(!EnsureRenderSurfaceForInput())
        {
            break;
        }

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
                renderer_->OnInput(input_state_);
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
            title_stream << (!title_.empty() ? title_ : L"DragonRenderer")
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
            input_state_.SetKeyDown(static_cast<uint32_t>(message_wparam), true);
        }
        break;
        case WM_KEYUP:
        {
            if(renderer_ == nullptr)
            {
                break;
            }
            input_state_.SetKeyDown(static_cast<uint32_t>(message_wparam), false);
        }
        break;
        case WM_LBUTTONDOWN:
        {
            input_state_.SetMousePosition(GET_X_LPARAM(message_lparam), GET_Y_LPARAM(message_lparam));
            input_state_.SetMouseButtonDown(InputState::MouseLeft, true);
            SetCapture(window_handler);
        }
        break;
        case WM_LBUTTONUP:
        {
            input_state_.SetMousePosition(GET_X_LPARAM(message_lparam), GET_Y_LPARAM(message_lparam));
            input_state_.SetMouseButtonDown(InputState::MouseLeft, false);
            ReleaseCapture();
        }
        break;
        case WM_MBUTTONDOWN:
        {
            input_state_.SetMousePosition(GET_X_LPARAM(message_lparam), GET_Y_LPARAM(message_lparam));
            input_state_.SetMouseButtonDown(InputState::MouseMiddle, true);
            SetCapture(window_handler);
        }
        break;
        case WM_MBUTTONUP:
        {
            input_state_.SetMousePosition(GET_X_LPARAM(message_lparam), GET_Y_LPARAM(message_lparam));
            input_state_.SetMouseButtonDown(InputState::MouseMiddle, false);
            ReleaseCapture();
        }
        break;
        case WM_RBUTTONDOWN:
        {
            input_state_.SetMousePosition(GET_X_LPARAM(message_lparam), GET_Y_LPARAM(message_lparam));
            input_state_.SetMouseButtonDown(InputState::MouseRight, true);
            SetCapture(window_handler);
        }
        break;
        case WM_RBUTTONUP:
        {
            input_state_.SetMousePosition(GET_X_LPARAM(message_lparam), GET_Y_LPARAM(message_lparam));
            input_state_.SetMouseButtonDown(InputState::MouseRight, false);
            ReleaseCapture();
        }
        break;
        case WM_MOUSEMOVE:
        {
            input_state_.SetMousePosition(GET_X_LPARAM(message_lparam), GET_Y_LPARAM(message_lparam));
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

bool WindowsApplication::ProcessPendingMessages()
{
    if(has_destoryed_)
    {
        return false;
    }

    MSG msg;
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if(has_destoryed_)
        {
            return false;
        }
    }

    return true;
}

void WindowsApplication::SwapBuffer()
{
    if(render_width_ == width_ && render_height_ == height_)
    {
        BitBlt(currentDC_, 0, 0, width_, height_, canvasDC_, 0, 0, SRCCOPY);
        return;
    }

    SetStretchBltMode(currentDC_, COLORONCOLOR);
    StretchBlt(currentDC_, 0, 0, width_, height_, canvasDC_, 0, 0, render_width_, render_height_, SRCCOPY);
}

bool WindowsApplication::EnsureRenderSurfaceForInput()
{
    if(!window_config_.enable_interactive_resolution_scale || renderer_ == nullptr)
    {
        return true;
    }

    auto now = std::chrono::steady_clock::now();
    if(IsInteractiveFrame())
    {
        last_interactive_time_ = now;
    }

    const auto idle_duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_interactive_time_).count();
    const bool should_use_interactive_surface =
        last_interactive_time_.time_since_epoch().count() != 0 &&
        idle_duration_ms <= static_cast<int64_t>(window_config_.interactive_recovery_ms);

    float target_scale = should_use_interactive_surface ? window_config_.interactive_render_scale : 1.0f;
    if(target_scale <= 0.0f || target_scale > 1.0f)
    {
        target_scale = 1.0f;
    }

    const int32_t target_width = std::max(1, static_cast<int32_t>(width_ * target_scale));
    const int32_t target_height = std::max(1, static_cast<int32_t>(height_ * target_scale));
    if(target_width == render_width_ && target_height == render_height_)
    {
        return true;
    }

    return ResizeRenderSurface(target_width, target_height);
}

bool WindowsApplication::ResizeRenderSurface(int32_t render_width, int32_t render_height)
{
    if(render_width <= 0 || render_height <= 0 || currentDC_ == nullptr)
    {
        return false;
    }

    if(canvasDC_ != nullptr && previous_canvas_object_ != nullptr)
    {
        SelectObject(canvasDC_, previous_canvas_object_);
        previous_canvas_object_ = nullptr;
    }

    if(bitmap_ != nullptr)
    {
        DeleteObject(bitmap_);
        bitmap_ = nullptr;
    }

    if(canvasDC_ != nullptr)
    {
        DeleteDC(canvasDC_);
        canvasDC_ = nullptr;
    }

    render_width_ = render_width;
    render_height_ = render_height;
    canvas_buffer_ = nullptr;

    canvasDC_ = CreateCompatibleDC(currentDC_);
    if(canvasDC_ == nullptr)
    {
        return false;
    }

    BITMAPINFO temp_bmp_info {};
    temp_bmp_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    temp_bmp_info.bmiHeader.biWidth = render_width_;
    temp_bmp_info.bmiHeader.biHeight = render_height_;
    temp_bmp_info.bmiHeader.biPlanes = 1;
    temp_bmp_info.bmiHeader.biBitCount = 32;
    temp_bmp_info.bmiHeader.biCompression = BI_RGB;

    bitmap_ = CreateDIBSection(canvasDC_, &temp_bmp_info, DIB_RGB_COLORS, (void**)&canvas_buffer_, 0, 0);
    if(bitmap_ == nullptr || canvas_buffer_ == nullptr)
    {
        if(bitmap_ != nullptr)
        {
            DeleteObject(bitmap_);
            bitmap_ = nullptr;
        }
        DeleteDC(canvasDC_);
        canvasDC_ = nullptr;
        canvas_buffer_ = nullptr;
        return false;
    }

    previous_canvas_object_ = SelectObject(canvasDC_, bitmap_);
    memset(canvas_buffer_, 0, render_width_ * render_height_ * 4);

    if(renderer_ != nullptr)
    {
        renderer_->ResizeRenderTarget(render_width_, render_height_, canvas_buffer_);
    }
    return true;
}

bool WindowsApplication::IsInteractiveFrame() const
{
    return input_state_.IsAnyKeyDown() ||
        input_state_.IsMouseButtonDown(InputState::MouseLeft) ||
        input_state_.IsMouseButtonDown(InputState::MouseMiddle) ||
        input_state_.IsMouseButtonDown(InputState::MouseRight);
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
        title_.c_str(),
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
    ResizeRenderSurface(render_width_, render_height_);
}

Application* CreateApplication()
{
    return new WindowsApplication();
}
