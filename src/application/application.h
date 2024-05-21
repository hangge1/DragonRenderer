#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <Windows.h>

class Application 
{
public:
    static Application* GetInstance();
public:
	Application() = default;
	~Application() = default;

    //注册、创建、显示窗口
    bool InitMainWindow(HINSTANCE program_instance, unsigned int main_window_width, unsigned int main_window_height);

	void ProcessMessage(HWND window_handler, UINT message_id, WPARAM message_wparam, LPARAM message_lparam);

	void DispatchMessageLoop();

    bool HasMainWindowDestoryed() const { return has_main_window_destoryed_;}
private:
    bool CreateMainWindow(HINSTANCE program_instance);
	ATOM RegisterMainWindowClass(HINSTANCE program_instance);

private:
    static Application* self_instance_;

    unsigned int main_window_width_;
    unsigned int main_window_height_;

    const WCHAR* main_window_class_name_ = L"DragonWindowClass";
    const CHAR* main_window_title_ = "DragonRenderer"; //用了宽字节，反而标题显示不正常，非要多字节然后强转

    HINSTANCE current_program_instance_;
    HWND main_window_handler_;

    bool has_main_window_destoryed_ = false;
};

#endif