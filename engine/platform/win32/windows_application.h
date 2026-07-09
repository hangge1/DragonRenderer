#ifndef _ENGINE_PLATFORM_WIN32_WINDOWS_APPLICATION_H_
#define _ENGINE_PLATFORM_WIN32_WINDOWS_APPLICATION_H_

#include "application.h"
#include "input_state.h"

#include <Windows.h>

#include <string>

class WindowsApplication final : public Application
{
public:
    WindowsApplication() = default;
    ~WindowsApplication() override;

    WindowsApplication(const WindowsApplication&) = delete;
    WindowsApplication(WindowsApplication&&) = delete;
    WindowsApplication& operator=(const WindowsApplication&) = delete;
    WindowsApplication& operator=(WindowsApplication&&) = delete;

    bool Init(void* platform_instance) override;

    void Run(const ApplicationRunOptions& options = {}) override;
    int32_t GetWidth() const override { return width_; }
    int32_t GetHeight() const override { return height_; }
    Renderer* GetRenderer() const override { return renderer_; }
    void RequestExit() override { has_destoryed_ = true; }

    void ProcessMessage(HWND window_handler, UINT message_id, WPARAM message_wparam, LPARAM message_lparam);

private:
    void InitDC();
    bool CreateMainWindow();
    void MoveWindow2DesktopCenter();
    ATOM RegisterMainWindowClass();
    void SwapBuffer();
    bool ProcessPendingMessages();

private:
    Renderer* renderer_ { nullptr };

    int32_t width_ { 0 };
    int32_t height_ { 0 };
    std::wstring title_;
    const wchar_t* register_class_name_ = L"DragonWindowClass";

    HINSTANCE hinstance_ { nullptr };
    HWND hwnd_ { nullptr };

    bool has_destoryed_ { false };
    InputState input_state_;

    HDC currentDC_ { nullptr };
    HDC canvasDC_ { nullptr };
    HBITMAP bitmap_ { nullptr };
    void* canvas_buffer_ { nullptr };
};

#endif
