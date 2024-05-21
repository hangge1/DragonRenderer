#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <Windows.h>

class Application 
{
public:
    static constexpr const LONG kDefaultWindowWidth = 800;
    static constexpr const LONG kDefaultWindowHeight = 600;
    static constexpr const CHAR* kDefaultWindowTitle = "DragonRenderer";
    static Application* GetInstance();
public:
	Application() = default;
	~Application() = default;

    //注册、创建、显示窗口
    bool InitMainWindow(HINSTANCE program_instance, 
        const CHAR* main_window_title = kDefaultWindowTitle,
        LONG main_window_width = kDefaultWindowWidth, 
        LONG main_window_height = kDefaultWindowHeight);

	void ProcessMessage(HWND window_handler, UINT message_id, WPARAM message_wparam, LPARAM message_lparam);

	void DispatchMessageLoop();

    bool HasMainWindowDestoryed() const { return has_main_window_destoryed_;}
private:
    bool CreateMainWindow();
	ATOM RegisterMainWindowClass();

private:
    static Application* self_instance_;

    LONG main_window_width_;
    LONG main_window_height_;
    const CHAR* main_window_title_;
    const WCHAR* main_window_class_name_ = L"DragonWindowClass";

    HINSTANCE current_program_instance_;
    HWND main_window_handler_;

    bool has_main_window_destoryed_ = false;
};

#endif