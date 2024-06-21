
#ifndef _APPLICATION_APPLICATION_H_
#define _APPLICATION_APPLICATION_H_

#define APP Application::GetInstance()

#include <Windows.h>
#include "camera.h"

class Application 
{
public:
    static constexpr const int32_t kDefaultWindowWidth = 800;
    static constexpr const int32_t kDefaultWindowHeight = 600;
    static constexpr const CHAR* kDefaultWindowTitle = "DragonRenderer";
    static Application* GetInstance();
public:
	Application() = default;
	~Application();

    //注册、创建、显示窗口
    bool InitMainWindow(HINSTANCE program_instance, 
        const CHAR* main_window_title = kDefaultWindowTitle,
        int32_t main_window_width = kDefaultWindowWidth, 
        int32_t main_window_height = kDefaultWindowHeight);

	void ProcessMessage(HWND window_handler, UINT message_id, WPARAM message_wparam, LPARAM message_lparam);

	bool DispatchMessageLoop();

    int32_t GetMainWindowWidth() const { return main_window_width_; }
    int32_t GetMainWindowHeight() const { return main_window_height_; }

    void* GetRenderBuffer() const { return canvas_buffer_;}

    void SetCamera(Camera* camera);
    Camera* GetCamera() const;
private:
    bool CreateMainWindow();

    void MoveWindow2DesktopCenter();

	ATOM RegisterMainWindowClass();

    void InitDrawContext();

    bool HasMainWindowDestoryed() const { return has_main_window_destoryed_; }

    void Render();

private:
    static Application* self_instance_;

    Camera* camera_ { nullptr };

    int32_t main_window_width_ {0};
    int32_t main_window_height_ {0};
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