/**
 * Author: 航火火
 * Path: main.cpp
 * Description: 
 *      Application Exe Main Entry Point File
*/

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup" )


#include <Windows.h>

#include <iostream>

#include "application.h"
#include "renderer.h"
#include "color.h"

void CustomDraw(Renderer& renderer)
{
    int width = Application::GetInstance()->GetMainWindowWidth();
    int height = Application::GetInstance()->GetMainWindowHeight();

    for (uint32_t i = 0; i < width; ++i) 
    {
		for (uint32_t j = 0; j < height; ++j) 
        {
			uint32_t v = std::rand() % 255;
			Color color(v, v, v, v);
			renderer.DrawOnePixel(i, j, color);
		}
	}
}

int WINAPI wWinMain(HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPWSTR lpCmdLine,
                    int nCmdShow)
{
    if(!Application::GetInstance()->InitMainWindow(hInstance))
    {
        std::cout << "Application Init Failed!" << std::endl;
        return -1;
    }

    Renderer renderer;
    renderer.InitFrameBuffer(
        Application::GetInstance()->GetMainWindowWidth(), 
        Application::GetInstance()->GetMainWindowHeight(),
        Application::GetInstance()->GetRenderBuffer());

    while(!Application::GetInstance()->HasMainWindowDestoryed())
    {
        Application::GetInstance()->DispatchMessageLoop();

        renderer.ClearFrameBuffer();

        //draw something
        CustomDraw(renderer);

        Application::GetInstance()->Render();
    }

    std::cout << "Application will exit!" << std::endl;

    return 0;
}


// 控制台入口函数
// int main()
// {
//     std::cout << "Hello Dragon Renderer" << std::endl;
//     return 0;
// }