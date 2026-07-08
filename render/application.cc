#include "application.h"


#include <chrono>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <windowsx.h>

#include "shader/lambert_light_shader.h"
#include "model.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "renderer.h"
#include "runtime/scoped_timer.h"

#include "event.h"

#include "perspective_camera.h"

LRESULT CALLBACK Wndproc(HWND window_handler, UINT message_id, WPARAM message_wparam, LPARAM message_lparam) 
{
	APP->ProcessMessage(window_handler, message_id, message_wparam, message_lparam);
	return(DefWindowProc(window_handler, message_id, message_wparam, message_lparam));
}

Application::~Application()
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
}

Application* Application::GetInstance() 
{
	static Application app;
	return &app;
}


bool Application::Init(HINSTANCE hinstance, const TCHAR* window_title,
	int32_t window_width, int32_t window_height) 
{
	width_ = window_width;
	height_ = window_height;
	hinstnce_ = hinstance;
	title_ = window_title;

	//初始化窗体类型，并且注册
	RegisterMainWindowClass();

	//生成一个窗体，并且显示
	if (!CreateMainWindow()) 
    {
		return false;
	}

	InitDC();

	renderer_ = new Renderer();
	renderer_->SetExitRequestedCallback([this]() { SetExit(); });
	renderer_->Init(width_, height_, canvas_buffer_);

	return true;
}

void Application::Run(const ApplicationRunOptions& options)
{
    using Clock = std::chrono::steady_clock;

    auto fps_sample_start = Clock::now();
    uint32_t sample_frame_count = 0;
    uint32_t total_frame_count = 0;

    double total_frame_ms = 0.0;
    double total_update_ms = 0.0;
    double total_render_ms = 0.0;
    double total_present_ms = 0.0;
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

		    //1、清空渲染帧(重新设置背景色, 重置深度缓冲区)
            renderer_->Clear();

		    //2、计算逻辑(摄像机, 光照等)
            {
                ScopedTimer update_timer(update_ms);
		        renderer_->OnUpdate();
            }

            //3、离屏渲染(渲染到用户创建的内存绘图设备上下文)
            {
                ScopedTimer render_timer(render_ms);
		        renderer_->Render();
            }

		    //4、交换到屏幕缓冲(离屏渲染结果拷贝到屏幕缓冲)
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
            title_stream << (title_ != nullptr ? title_ : TEXT("DragonRenderer"))
                << TEXT(" | FPS: ") << std::fixed << std::setprecision(1) << fps
                << TEXT(" | Frame: ") << std::fixed << std::setprecision(2) << stats.frame_ms << TEXT(" ms")
                << TEXT(" | U/R/P: ") << std::fixed << std::setprecision(2)
                << stats.update_ms << TEXT("/") << stats.render_ms << TEXT("/") << stats.present_ms << TEXT(" ms")
                << TEXT(" | DC: ") << stats.draw_calls
                << TEXT(" | Tri: ") << stats.input_triangles
                << TEXT(" | Frag: ") << stats.rasterized_fragments;
            SetWindowText(hwnd_, title_stream.str().c_str());

            sample_frame_count = 0;
            fps_sample_start = now;
        }

        if(options.max_frames > 0 && total_frame_count >= options.max_frames)
        {
            SetExit();
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
        std::cout << "Average draw calls: " << total_draw_calls / count << std::endl;
        std::cout << "Average input triangles: " << total_input_triangles / count << std::endl;
        std::cout << "Average rasterized fragments: " << total_rasterized_fragments / count << std::endl;
    }

	delete renderer_;
	renderer_ = nullptr;
}


void Application::ProcessMessage(HWND window_handler, UINT message_id, WPARAM message_wparam, LPARAM message_lparam) 
{
	switch (message_id)
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
			MouseDownEvent ev(GET_X_LPARAM(message_lparam), GET_Y_LPARAM(message_lparam),2);
			renderer_->OnEvent(ev);
		}
		break;
		case WM_RBUTTONUP:
		{
			if(renderer_ == nullptr)
			{
				break;
			}
			MouseUpEvent ev(GET_X_LPARAM(message_lparam), GET_Y_LPARAM(message_lparam),2);
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
			DestroyWindow(window_handler);//此处销毁窗体,会自动发出WM_DESTROY
		}
		break;			
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(window_handler, &ps);
			EndPaint(window_handler, &ps);
		}
		break;
		case WM_DESTROY: 
		{
			has_destoryed_ = true;
			PostQuitMessage(0);//发出线程终止请求
		}
		break;
	}
}


bool Application::IsInLoop() 
{
	if(has_destoryed_)
	{
		return false;
	}

	MSG msg;
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}


void Application::SwapBuffer()
{
	BitBlt(currentDC_, 0, 0, width_, height_, canvasDC_, 0, 0, SRCCOPY);
}

bool Application::CreateMainWindow()
{
	/*
	* WS_POPUP: 不需要标题栏，则不需要边框
	* WS_OVERLAPPEDWINDOW：拥有普通程序主窗口的所有特点，必须有标题且有边框
	*
	* WS_CLIPSIBLINGS: 被兄弟窗口挡住区域不绘制
	* WS_CLIPCHILDREN: 被子窗口遮挡住的区域不绘制
	*/

	auto dwExStyle = WS_EX_APPWINDOW;
	auto dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	//由于存在标题栏等，所以需要计算中间显示区域的大小,比如PopUp的窗体，就没有标题栏，则不会改变
	RECT windowRect;
	windowRect.left = 0L;
	windowRect.top = 0L;
	windowRect.right = (LONG)width_;
	windowRect.bottom = (LONG)height_;
	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

	hwnd_ = CreateWindow(
		register_class_name_,
		title_,  //窗体标题，强转的目的: 保证标题正常显示
		dwStyle,
		500,  //x位置，相对左上角
		500,  //y位置，相对左上角
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,  //父窗体
		nullptr,  //菜单栏
		hinstnce_,//程序实例
		nullptr);  //额外参数


	if (!hwnd_)
	{
		return false;
	}

	MoveWindow2DesktopCenter();

	ShowWindow(hwnd_, true);

	UpdateWindow(hwnd_);

	return true;
}

void Application::MoveWindow2DesktopCenter()
{
	RECT rectWindow;
	// 获取窗口的尺寸
    GetWindowRect(hwnd_, &rectWindow);
    
	RECT rectScreen;
    // 获取屏幕的尺寸
    GetClientRect(GetDesktopWindow(), &rectScreen);

    // 计算居中位置
    int posX = (rectScreen.right - rectWindow.right + rectWindow.left) / 2;
    int posY = (rectScreen.bottom - rectWindow.bottom + rectWindow.top) / 2;

    // 移动窗口到新位置
    SetWindowPos(hwnd_, HWND_TOP, posX, posY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

ATOM Application::RegisterMainWindowClass()
{
	WNDCLASSEX wndClass;

	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	//水平/垂直大小发生变化重绘窗口
	wndClass.lpfnWndProc = Wndproc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hinstnce_;		//应用程序句柄
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);//应用程序图标,即任务栏的大图标
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);//鼠标图标
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);//窗口背景色
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = register_class_name_;//窗口类名
	wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);//窗口标题图标

	return RegisterClassEx(&wndClass);
}


void Application::InitDC()
{
	currentDC_ = GetDC(hwnd_);
	canvasDC_ = CreateCompatibleDC(currentDC_); //创建内存绘图设备上下文, 主要用于双缓冲

	BITMAPINFO  temp_bmp_info {};
	temp_bmp_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	temp_bmp_info.bmiHeader.biWidth = width_;
	temp_bmp_info.bmiHeader.biHeight = height_;
	temp_bmp_info.bmiHeader.biPlanes = 1;
	temp_bmp_info.bmiHeader.biBitCount = 32;
	temp_bmp_info.bmiHeader.biCompression = BI_RGB; //实际上存储方式为bgra

	//创建一个设备无关的位图,它可以在不同设备上下文共享，并且可以直接访问其位图数据! canvas_buffer_就是位图数据
	bitmap_ = CreateDIBSection(canvasDC_, &temp_bmp_info, DIB_RGB_COLORS, (void**)&canvas_buffer_, 0, 0);

	//一个设备可以创建多个位图，本设备使用bitmap_作为激活位图，对canvasDC_的内存拷出，其实就是拷出了bitmap_的数据
	SelectObject(canvasDC_, bitmap_);

	memset(canvas_buffer_, 0, width_ * height_ * 4); //清空buffer为0
}
