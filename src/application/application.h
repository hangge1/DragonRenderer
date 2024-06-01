
#ifndef _APPLICATION_APPLICATION_H_
#define _APPLICATION_APPLICATION_H_

#define APP Application::GetInstance()

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
	~Application();

    //注册、创建、显示窗口
    bool InitMainWindow(HINSTANCE program_instance, 
        const CHAR* main_window_title = kDefaultWindowTitle,
        LONG main_window_width = kDefaultWindowWidth, 
        LONG main_window_height = kDefaultWindowHeight);

	void ProcessMessage(HWND window_handler, UINT message_id, WPARAM message_wparam, LPARAM message_lparam);

	void DispatchMessageLoop();

    bool HasMainWindowDestoryed() const { return has_main_window_destoryed_; }

    LONG GetMainWindowWidth() const { return main_window_width_; }
    LONG GetMainWindowHeight() const { return main_window_height_; }

    void* GetRenderBuffer() const { return canvas_buffer_;}

    void Render();
private:
    bool CreateMainWindow();

    void MoveWindow2DesktopCenter();

	ATOM RegisterMainWindowClass();

    void InitDrawContext();
private:
    static Application* self_instance_;

    LONG main_window_width_ {0};
    LONG main_window_height_ {0};
    const CHAR* main_window_title_ { nullptr };
    const WCHAR* main_window_class_name_ = L"DragonWindowClass";

    HINSTANCE current_program_instance_ { nullptr };
    HWND main_window_handler_ { nullptr };

    bool has_main_window_destoryed_ = false;

    HDC current_window_device_context_ { nullptr };
    HDC canvas_device_contex_ { nullptr };
    HBITMAP bitmap_ { nullptr };
    void* canvas_buffer_ { nullptr };
};

#endif