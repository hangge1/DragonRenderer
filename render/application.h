
#ifndef _APPLICATION_APPLICATION_H_
#define _APPLICATION_APPLICATION_H_

#define APP Application::GetInstance()

#include <Windows.h>
#include <stdint.h>


class Renderer;

class Application final
{
public:
    static constexpr int32_t kDefaultWindowWidth = 800;
    static constexpr int32_t kDefaultWindowHeight = 600;
    static constexpr TCHAR* kDefaultWindowTitle = TEXT("DragonRenderer");

    static Application* GetInstance();

    //注册、创建、显示窗口
    bool Init(HINSTANCE program_instance, 
        const TCHAR* main_window_title = kDefaultWindowTitle,
        int32_t main_window_width = kDefaultWindowWidth, 
        int32_t main_window_height = kDefaultWindowHeight);
    
    //运行渲染循环
    void Run();

    inline int32_t GetWidth() const { return width_; }
    inline int32_t GetHeight() const { return height_; }

    void SetExit() { has_destoryed_ = true; }

    //WindownProc使用
    void ProcessMessage(HWND window_handler, UINT message_id, WPARAM message_wparam, LPARAM message_lparam);
private:
	Application() = default;
	~Application();  

    Application(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&) = delete;

private:
    void InitDC();

    bool CreateMainWindow();

    void MoveWindow2DesktopCenter();

	ATOM RegisterMainWindowClass();

    void SwapBuffer();

	bool IsInLoop();
private:
    Renderer* renderer_;

    int32_t width_ { 0 };
    int32_t height_ { 0 };
    const TCHAR* title_ { nullptr };
    const TCHAR* register_class_name_ = TEXT("DragonWindowClass");

    HINSTANCE hinstnce_ { nullptr };
    HWND hwnd_ { nullptr };

    bool has_destoryed_ = false;

    HDC currentDC_ { nullptr };
    HDC canvasDC_ { nullptr };
    HBITMAP bitmap_ { nullptr };
    void* canvas_buffer_ { nullptr };
};

#endif