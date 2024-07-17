
#ifndef _APPLICATION_APPLICATION_H_
#define _APPLICATION_APPLICATION_H_

#define APP Application::GetInstance()

#include <Windows.h>
#include <stdint.h>

#include "glm/glm.hpp"


class Renderer;

class Application 
{
public:
    static constexpr const int32_t kDefaultWindowWidth = 800;
    static constexpr const int32_t kDefaultWindowHeight = 600;
    static constexpr const TCHAR* kDefaultWindowTitle = TEXT("DragonRenderer");
    static Application* GetInstance();
public:
	Application() = default;
	~Application();

    //注册、创建、显示窗口
    bool Init(HINSTANCE program_instance, 
        const TCHAR* main_window_title = kDefaultWindowTitle,
        int32_t main_window_width = kDefaultWindowWidth, 
        int32_t main_window_height = kDefaultWindowHeight);


    void Run();

	void ProcessMessage(HWND window_handler, UINT message_id, WPARAM message_wparam, LPARAM message_lparam);

	bool DispatchMessageLoop();

    int32_t GetMainWindowWidth() const { return window_width_; }
    int32_t GetMainWindowHeight() const { return window_height_; }

    void* GetRenderBuffer() const { return canvas_buffer_;}



    //获取鼠标位置
    glm::vec2 GetCursorPosition();
private:
    bool CreateMainWindow();

    void MoveWindow2DesktopCenter();

	ATOM RegisterMainWindowClass();

    void InitDC();

    bool HasMainWindowDestoryed() const { return has_destoryed_; }

    void SwapBuffer();

private:
    static Application* self_instance_;
    Renderer* renderer_;


    int32_t window_width_ { 0 };
    int32_t window_height_ { 0 };
    const TCHAR* window_title_ { nullptr };
    const TCHAR* register_class_name_ = TEXT("DragonWindowClass");

    HINSTANCE hinstnce_ { nullptr };
    HWND hwnd_ { nullptr };

    bool has_destoryed_ = false;

    HDC currentDC { nullptr };
    HDC canvasDC { nullptr };
    HBITMAP bitmap_ { nullptr };
    void* canvas_buffer_ { nullptr };
};

#endif