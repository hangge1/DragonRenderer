#include "application.h"


#include <windowsx.h>

#include "shader/lambert_light_shader.h"
#include "model.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "renderer.h"

#include "event.h"

#include "perspective_camera.h"

LRESULT CALLBACK Wndproc(HWND window_handler, UINT message_id, WPARAM message_wparam, LPARAM message_lparam) 
{
	APP->ProcessMessage(window_handler, message_id, message_wparam, message_lparam);
	return(DefWindowProc(window_handler, message_id, message_wparam, message_lparam));
}

Application* Application::self_instance_ = nullptr;

Application::~Application()
{
	if(nullptr != bitmap_)
	{
		DeleteObject(bitmap_);
	}

	if(nullptr != canvasDC)
	{
		DeleteDC(canvasDC);
	}

	if(nullptr != currentDC && nullptr != hwnd_)
	{
		ReleaseDC(hwnd_, currentDC);
	}
}

Application* Application::GetInstance() 
{
	if (nullptr == self_instance_) 
	{
		self_instance_ = new Application();
	}

	return self_instance_;
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
	renderer_->Init(width_, height_, canvas_buffer_);

	return true;
}

void Application::Run()
{
    while(IsInLoop())
    {
		//1、清空渲染帧
        renderer_->Clear();

		//2、计算逻辑
		renderer_->OnUpdate();

        //3、离屏渲染
		renderer_->Render();

		//4、交换到屏幕缓冲
		SwapBuffer();
    }
}


void Application::ProcessMessage(HWND window_handler, UINT message_id, WPARAM message_wparam, LPARAM message_lparam) 
{
	switch (message_id)
	{
		case WM_KEYDOWN:
		{
			KeyDownEvent ev(message_wparam);
			renderer_->OnEvent(ev);
		}
		break;
		case WM_KEYUP:
		{
			KeyUpEvent ev(message_wparam);
			renderer_->OnEvent(ev);
		}
		break;
		case WM_RBUTTONDOWN:
		{
			MouseDownEvent ev(GET_X_LPARAM(message_lparam), GET_Y_LPARAM(message_lparam),2);
			renderer_->OnEvent(ev);
		}
		break;
		case WM_RBUTTONUP:
		{
			MouseUpEvent ev(GET_X_LPARAM(message_lparam), GET_Y_LPARAM(message_lparam),2);
			renderer_->OnEvent(ev);
		}
		break;
		case WM_MOUSEMOVE:
		{
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
	if(IsExit())
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
	BitBlt(currentDC, 0, 0, width_, height_, canvasDC, 0, 0, SRCCOPY);
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
	currentDC = GetDC(hwnd_);
	canvasDC = CreateCompatibleDC(currentDC);

	BITMAPINFO  temp_bmp_info{};
	temp_bmp_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	temp_bmp_info.bmiHeader.biWidth = width_;
	temp_bmp_info.bmiHeader.biHeight = height_;
	temp_bmp_info.bmiHeader.biPlanes = 1;
	temp_bmp_info.bmiHeader.biBitCount = 32;
	temp_bmp_info.bmiHeader.biCompression = BI_RGB; //实际上存储方式为bgra

	//创建与mhMem兼容的位图,其实实在mhMem指代的设备上划拨了一块内存，让mCanvasBuffer指向它
	bitmap_ = CreateDIBSection(canvasDC, &temp_bmp_info, DIB_RGB_COLORS, (void**)&canvas_buffer_, 0, 0);//在这里创建buffer的内存

	//一个设备可以创建多个位图，本设备使用mhBmp作为激活位图，对mCanvasDC的内存拷出，其实就是拷出了mhBmp的数据
	SelectObject(canvasDC, bitmap_);

	memset(canvas_buffer_, 0, width_ * height_ * 4); //清空buffer为0
}