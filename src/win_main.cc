/*
    Author: 航火火
    Path: main.cpp
    Description:  Application Exe Main Entry Point File
*/

#include <Windows.h>

#include <iostream>

#include "application.h"
#include "renderer.h"
#include "pixel.h"
#include "image.h"

//渲染随机像素值，形成类似老式电视机的那种雪花图
void RenderRandomPixel(Renderer& renderer)
{
    int width = APP->GetMainWindowWidth();
    int height = APP->GetMainWindowHeight();

    for (uint32_t i = 0; i < width; ++i) 
    {
		for (uint32_t j = 0; j < height; ++j) 
        {
			u_char v = std::rand() % 255;
			Color color(v, v, v, v);
			renderer.DrawPixel(i, j, color);
		}
	}
}

//渲染一条白色直线
void RenderOneLine(Renderer& renderer)
{
    Pixel start(300, 300);
    Pixel end(400, 400);

    renderer.DrawLine(start, end);
}

//渲染一条由红到绿的直线
void RenderOneColorLine(Renderer& renderer)
{
    Pixel start(300, 300, Color(255, 0, 0, 255));
    Pixel end(400, 400, Color(0, 255, 0, 255));

    renderer.DrawLine(start, end);
}

//渲染白色的三角形
void RenderOneTriangle(Renderer& renderer)
{
    Pixel p1(300, 300, Color(255, 255, 255, 255));
    Pixel p2(500, 300, Color(255, 255, 255, 255));
    Pixel p3(400, 500, Color(255, 255, 255, 255));

    renderer.DrawTriangle(p1, p2, p3);
}

//渲染三个顶点颜色为红绿蓝的三角形
void RenderOneColorTriangle(Renderer& renderer)
{
    Pixel p1(300, 300, Color(255, 0, 0, 255));
    Pixel p2(500, 300, Color(0, 255, 0, 255));
    Pixel p3(400, 500, Color(0, 0, 255, 255));

    renderer.DrawTriangle(p1, p2, p3);
}


Image* lufei_image = Image::createImage(ASSETS_PATH "/texture/lufei.jpg");
//Image* chair_image = Image::createImage(ASSETS_PATH "/texture/chair.png");
Image* chair_image = Image::createImage(ASSETS_PATH "/texture/chair_blend.png");

//渲染图片
void RenderPicture(Renderer& renderer)
{
    renderer.DrawPicture(*lufei_image);
}

//渲染混合，方法1
void RenderBlendPicture(Renderer& renderer)
{
    renderer.DrawPicture(*lufei_image);

    renderer.SetColorBlend(true);

    int width = chair_image->get_width();
    int height = chair_image->get_height();
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            chair_image->get_data()[j * width + i].a = 200;
        }
    }
    

    renderer.DrawPicture(*chair_image);

    renderer.SetColorBlend(false);

}

//渲染混合，方法2
void RenderBlendPicture2(Renderer& renderer)
{
    renderer.DrawPicture(*lufei_image);
    renderer.DrawPictureOnBlend(*chair_image, 200);
}



void CustomDraw(Renderer& renderer)
{
    //RenderRandomPixel(renderer);
    //RenderOneLine(renderer);
    //RenderOneColorLine(renderer);
    //RenderOneTriangle(renderer);
    //RenderOneColorTriangle(renderer);

    //RenderPicture(renderer);
    //RenderBlendPicture(renderer);
    RenderBlendPicture2(renderer);
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

    Image::destroyImage(lufei_image);

    return 0;
}


// 控制台入口函数
// int main()
// {
//     std::cout << "Hello Dragon Renderer" << std::endl;
//     return 0;
// }