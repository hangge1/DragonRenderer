/**
 * Author: 航火火
 * Path: main.cpp
 * Description: 
 *      Application Exe Main Entry Point File
*/

#include <Windows.h>

#include <iostream>

#include "application.h"
#include "renderer.h"
#include "color.h"
#include "pixel.h"

//渲染随机像素值，形成类似老式电视机的那种雪花图
void RenderRandomPixel(Renderer& renderer)
{
    int width = APP->GetMainWindowWidth();
    int height = APP->GetMainWindowHeight();

    for (uint32_t i = 0; i < width; ++i) 
    {
		for (uint32_t j = 0; j < height; ++j) 
        {
			uint32_t v = std::rand() % 255;
			Color color(v, v, v, v);
			renderer.DrawPixel(i, j, color);
		}
	}
}

//渲染一条白色直线
void RenderOneLine(Renderer& renderer)
{
    Pixel start {300, 300};
    Pixel end {500, 600};

    renderer.DrawLine(start, end);
}

void CustomDraw(Renderer& renderer)
{
    //RenderRandomPixel(renderer);

    RenderOneLine(renderer);
}

int WINAPI wWinMain(HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPWSTR lpCmdLine,
                    int nCmdShow)
{
    if(!APP->InitMainWindow(hInstance))
    {
        std::cout << "Application Init Failed!" << std::endl;
        return -1;
    }

    Renderer renderer;
    renderer.InitFrameBuffer(
        APP->GetMainWindowWidth(), 
        APP->GetMainWindowHeight(),
        APP->GetRenderBuffer());

    while(!APP->HasMainWindowDestoryed())
    {
        APP->DispatchMessageLoop();

        renderer.ClearFrameBuffer();

        //draw something
        CustomDraw(renderer);

        APP->Render();
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