#include "application.h"

#include <Windows.h>

Application* Application::self_instance_ = nullptr;

Application* Application::GetInstance() 
{
	if (nullptr == self_instance_) 
	{
		self_instance_ = new Application();
	}

	return self_instance_;
}

LRESULT CALLBACK Wndproc(HWND window_handler, UINT message_id, WPARAM message_wparam, LPARAM message_lparam) 
{
	Application::GetInstance()->ProcessMessage(window_handler, message_id, message_wparam, message_lparam);
	return(DefWindowProc(window_handler, message_id, message_wparam, message_lparam));
}

bool Application::InitMainWindow(HINSTANCE program_instance, 
	unsigned int main_window_width, unsigned int main_window_height) 
{
	main_window_width_ = main_window_width;
	main_window_height_ = main_window_height;

	//初始化窗体类型，并且注册
	RegisterMainWindowClass(program_instance);

	//生成一个窗体，并且显示
	if (!CreateMainWindow(program_instance)) 
    {
		return false;
	}

	return true;
}

ATOM Application::RegisterMainWindowClass(HINSTANCE program_instance)
{
	WNDCLASSEXW wndClass;

	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	//水平/垂直大小发生变化重绘窗口
	wndClass.lpfnWndProc = Wndproc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = program_instance;		//应用程序句柄
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);//应用程序图标,即任务栏的大图标
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);//鼠标图标
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);//窗口背景色
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = main_window_class_name_;//窗口类名
	wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);//窗口标题图标

	return RegisterClassExW(&wndClass);
}

bool Application::CreateMainWindow(HINSTANCE program_instance)
{
	current_program_instance_ = program_instance;

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
	windowRect.right = (LONG)main_window_width_;
	windowRect.bottom = (LONG)main_window_height_;
	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

	main_window_handler_ = CreateWindowW(
		main_window_class_name_,
		(LPCWSTR)main_window_title_,  //窗体标题，强转的目的: 保证标题正常显示
		dwStyle,
		500,  //x位置，相对左上角
		500,  //y位置，相对左上角
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,  //父窗体
		nullptr,  //菜单栏
		current_program_instance_,//程序实例
		nullptr);  //额外参数


	if (!main_window_handler_)
	{
		return false;
	}

	ShowWindow(main_window_handler_, true);

	UpdateWindow(main_window_handler_);

	return true;
}

void Application::DispatchMessageLoop() 
{
	MSG msg;
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void Application::ProcessMessage(HWND window_handler, UINT message_id, WPARAM message_wparam, LPARAM message_lparam) 
{
	switch (message_id)
	{
		case WM_CLOSE: 
		{
			DestroyWindow(window_handler);//此处销毁窗体,会自动发出WM_DESTROY
			break;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(window_handler, &ps);
			EndPaint(window_handler, &ps);
		}
		break;
		case WM_DESTROY: 
		{
			has_main_window_destoryed_ = true;
			PostQuitMessage(0);//发出线程终止请求
			break;
		}
	}
}