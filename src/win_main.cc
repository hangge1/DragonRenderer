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
#include "glm/ext.hpp"

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
Image* goku_image = Image::createImage(ASSETS_PATH "/texture/goku.jpg");
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

//最邻近纹理采样
void RenderTriangleByNearestSampleTexture(Renderer& renderer)
{
    renderer.SetTexture(lufei_image);

    Pixel p1(300, 300, Color(255, 0, 0, 255), 0.0f, 0.0f);
    Pixel p2(500, 300, Color(0, 255, 0, 255), 1.0f, 0.0f);
    Pixel p3(400, 500, Color(0, 0, 255, 255), 0.5f, 1.0f);

    renderer.DrawTriangle(p1, p2, p3);
}

//双线性插值采样
void RenderTriangleByBilinearSampleTexture(Renderer& renderer)
{
    renderer.SetTexture(goku_image);
    renderer.SetBilinearSample(true);

    Pixel p1(0, 0, Color(255, 0, 0, 255), 0.0f, 0.0f);
    Pixel p2(400, 0, Color(0, 255, 0, 255), 1.0f, 0.0f);
    Pixel p3(200, 600, Color(0, 0, 255, 255), 0.5f, 1.0f);

    renderer.DrawTriangle(p1, p2, p3);

    renderer.SetBilinearSample(false);

    Pixel p4(400, 0, Color(255, 0, 0, 255), 0.0f, 0.0f);
    Pixel p5(800, 0, Color(0, 255, 0, 255), 1.0f, 0.0f);
    Pixel p6(600, 600, Color(0, 0, 255, 255), 0.5f, 1.0f);

    renderer.DrawTriangle(p4, p5, p6);
}

//Repeat Wrap测试
void RenderTextureRepeatWrap(Renderer& renderer)
{
    renderer.SetTexture(goku_image);
    renderer.SetBilinearSample(true);
    renderer.SetTextureUvWrap(WrapRepeat);

    Pixel p1(0, 0, Color(255, 0, 0, 255), 0.0f, 0.0f);
    Pixel p2(400, 0, Color(0, 255, 0, 255), 1.0f, 0.0f);
    Pixel p3(200, 600, Color(0, 0, 255, 255), 0.5f, 1.0f);

    renderer.DrawTriangle(p1, p2, p3);

    Pixel p4(400, 0, Color(255, 0, 0, 255), 1.0f, 0.0f);
    Pixel p5(800, 0, Color(0, 255, 0, 255), 2.0f, 0.0f);
    Pixel p6(600, 600, Color(0, 0, 255, 255), 1.5f, 1.0f);

    renderer.DrawTriangle(p4, p5, p6);
}

//Mirror Wrap测试
void RenderTextureMirrorWrap(Renderer& renderer)
{
    renderer.SetTexture(goku_image);
    renderer.SetBilinearSample(true);
    renderer.SetTextureUvWrap(WrapMirror);

    Pixel p1(0, 0, Color(255, 0, 0, 255), 0.0f, 0.0f);
    Pixel p2(400, 0, Color(0, 255, 0, 255), 1.0f, 0.0f);
    Pixel p3(200, 600, Color(0, 0, 255, 255), 0.5f, 1.0f);

    renderer.DrawTriangle(p1, p2, p3);

    Pixel p4(400, 0, Color(255, 0, 0, 255), 1.0f, 0.0f);
    Pixel p5(800, 0, Color(0, 255, 0, 255), 2.0f, 0.0f);
    Pixel p6(600, 600, Color(0, 0, 255, 255), 1.5f, 1.0f);

    renderer.DrawTriangle(p4, p5, p6);
}

//Mvp 变换矩阵测试(默认的能显示)
void RenderTransform1(Renderer& renderer)
{
    glm::vec4 p1(-5.0f, 0.0f, -5.0f, 1.0f);
    glm::vec4 p2(5.0f, 0.0f, -5.0f, 1.0f);
    glm::vec4 p3(0.0f, 5.0f, 5.0f, 1.0f);

    glm::mat4 identity = glm::identity<glm::mat4>();
    glm::mat4 model_matrix = glm::translate(identity, glm::vec3(0.0f, 0.0f, 0.0f));

    glm::mat4 camera_model_matrix = glm::translate(identity, glm::vec3(0.0f, 0.0f, 5.0f));
    glm::mat4 view_matrix = glm::inverse(camera_model_matrix);

    glm::mat4 project_matrix = glm::ortho(-10.0f, 10.0f, -3.0f, 10.0f, 15.0f, -30.0f);

    int screen_width = APP->GetMainWindowWidth();
    int screen_height = APP->GetMainWindowHeight();
    glm::mat4 screen_matrix = {
        screen_width / 2, 0.0f, 0.0f, screen_width / 2,
        0.0f, screen_height / 2, 0.0f, screen_height / 2,
        0.0f, 0.0f, 0.5f, 0.5f, 
        0.0f, 0.0f, 0.0f, 1.0f
    };

    screen_matrix = glm::transpose(screen_matrix);

    glm::vec4 clip_p1 = project_matrix * view_matrix * model_matrix * p1;
    glm::vec4 clip_p2 = project_matrix * view_matrix * model_matrix * p2;
    glm::vec4 clip_p3 = project_matrix * view_matrix * model_matrix * p3;

    clip_p1 /= clip_p1.w;
    clip_p2 /= clip_p2.w;
    clip_p3 /= clip_p3.w;

    clip_p1 = screen_matrix * clip_p1;
    clip_p2 = screen_matrix * clip_p2;
    clip_p3 = screen_matrix * clip_p3;

    renderer.SetTexture(goku_image);
    renderer.SetBilinearSample(true);

    Pixel pos1(clip_p1.x, clip_p1.y, Color(255, 0, 0, 255), 0.0f, 0.0f);
    Pixel pos2(clip_p2.x, clip_p2.y, Color(0, 255, 0, 255), 1.0f, 0.0f);
    Pixel pos3(clip_p3.x, clip_p3.y, Color(0, 0, 255, 255), 0.5f, 1.0f);

    renderer.DrawTriangle(pos1, pos2, pos3);
}

//Mvp 变换矩阵测试(物体绕y轴旋转)
void RenderTransform2(Renderer& renderer)
{
    glm::vec4 p1(-5.0f, 0.0f, -5.0f, 1.0f);
    glm::vec4 p2(5.0f, 0.0f, -5.0f, 1.0f);
    glm::vec4 p3(0.0f, 5.0f, 5.0f, 1.0f);

    static float angle = 10;
    angle++;
    float rangle = glm::radians(angle);

    glm::mat4 identity = glm::identity<glm::mat4>();
    glm::mat4 model_matrix = glm::rotate(identity, rangle, glm::vec3(0.0f, 1.0f, 0.0f));//glm::translate(identity, glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 view_matrix = glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 project_matrix = glm::ortho(-10.0f, 10.0f, -3.0f, 10.0f, 15.0f, -30.0f);

    int screen_width = APP->GetMainWindowWidth();
    int screen_height = APP->GetMainWindowHeight();
    glm::mat4 screen_matrix = {
        screen_width / 2, 0.0f, 0.0f, screen_width / 2,
        0.0f, screen_height / 2, 0.0f, screen_height / 2,
        0.0f, 0.0f, 0.5f, 0.5f, 
        0.0f, 0.0f, 0.0f, 1.0f
    };

    screen_matrix = glm::transpose(screen_matrix);

    glm::vec4 clip_p1 = project_matrix * view_matrix * model_matrix * p1;
    glm::vec4 clip_p2 = project_matrix * view_matrix * model_matrix * p2;
    glm::vec4 clip_p3 = project_matrix * view_matrix * model_matrix * p3;

    clip_p1 /= clip_p1.w;
    clip_p2 /= clip_p2.w;
    clip_p3 /= clip_p3.w;

    clip_p1 = screen_matrix * clip_p1;
    clip_p2 = screen_matrix * clip_p2;
    clip_p3 = screen_matrix * clip_p3;

    renderer.SetTexture(goku_image);
    renderer.SetBilinearSample(true);

    Pixel pos1(clip_p1.x, clip_p1.y, Color(255, 0, 0, 255), 0.0f, 0.0f);
    Pixel pos2(clip_p2.x, clip_p2.y, Color(0, 255, 0, 255), 1.0f, 0.0f);
    Pixel pos3(clip_p3.x, clip_p3.y, Color(0, 0, 255, 255), 0.5f, 1.0f);

    renderer.DrawTriangle(pos1, pos2, pos3);
}

//Mvp 变换矩阵测试(相机绕y轴旋转)
void RenderTransform3(Renderer& renderer)
{
    glm::vec4 p1(-5.0f, 0.0f, -5.0f, 1.0f);
    glm::vec4 p2(5.0f, 0.0f, -5.0f, 1.0f);
    glm::vec4 p3(0.0f, 5.0f, 5.0f, 1.0f);

    static float angle = 10;
    angle++;
    float rangle = glm::radians(angle);

    glm::mat4 identity = glm::identity<glm::mat4>();
    glm::vec4 cameraPos = glm::rotate(identity, rangle, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    glm::mat4 model_matrix = glm::translate(identity, glm::vec3(0.0f, 0.0f, 0.0f));
    
    glm::mat4 view_matrix = glm::lookAt(glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    glm::mat4 project_matrix = glm::ortho(-10.0f, 10.0f, -3.0f, 10.0f, 15.0f, -30.0f);

    int screen_width = APP->GetMainWindowWidth();
    int screen_height = APP->GetMainWindowHeight();
    glm::mat4 screen_matrix = {
        screen_width / 2, 0.0f, 0.0f, screen_width / 2,
        0.0f, screen_height / 2, 0.0f, screen_height / 2,
        0.0f, 0.0f, 0.5f, 0.5f, 
        0.0f, 0.0f, 0.0f, 1.0f
    };

    screen_matrix = glm::transpose(screen_matrix);

    glm::vec4 clip_p1 = project_matrix * view_matrix * model_matrix * p1;
    glm::vec4 clip_p2 = project_matrix * view_matrix * model_matrix * p2;
    glm::vec4 clip_p3 = project_matrix * view_matrix * model_matrix * p3;

    clip_p1 /= clip_p1.w;
    clip_p2 /= clip_p2.w;
    clip_p3 /= clip_p3.w;

    clip_p1 = screen_matrix * clip_p1;
    clip_p2 = screen_matrix * clip_p2;
    clip_p3 = screen_matrix * clip_p3;

    renderer.SetTexture(goku_image);
    renderer.SetBilinearSample(true);

    Pixel pos1(clip_p1.x, clip_p1.y, Color(255, 0, 0, 255), 0.0f, 0.0f);
    Pixel pos2(clip_p2.x, clip_p2.y, Color(0, 255, 0, 255), 1.0f, 0.0f);
    Pixel pos3(clip_p3.x, clip_p3.y, Color(0, 0, 255, 255), 0.5f, 1.0f);

    renderer.DrawTriangle(pos1, pos2, pos3);
}

void RenderByBindVAO(Renderer& renderer)
{
    // static uint32_t vbo = renderer.GenBuffer();
    // renderer.DeleteBuffer(vbo);

    // static uint32_t vao = renderer.GenVertexArray();
    // renderer.DeleteVertexArray(vao);


    float positions[] = {
		-0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
	};

	float colors[] = {
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
	};

	float uvs[] = {
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
	};

	uint32_t indices[] = { 0, 1, 2 };

	//生成indices对应ebo
	static auto ebo = renderer.GenBuffer();
	renderer.BindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
	renderer.BufferData(ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 3, indices);
	renderer.BindBuffer(ELEMENT_ARRAY_BUFFER, 0);

	//生成vao并且绑定
	static auto vao = renderer.GenVertexArray();
	renderer.BindVertexArray(vao);
	
	//生成每个vbo，绑定后，设置属性ID及读取参数
	static auto positionVbo = renderer.GenBuffer();
	renderer.BindBuffer(ARRAY_BUFFER, positionVbo);
	renderer.BufferData(ARRAY_BUFFER, sizeof(float) * 9, positions);
	renderer.VertexAttributePointer(0, 3, 3 * sizeof(float), 0);

	static auto colorVbo = renderer.GenBuffer();
	renderer.BindBuffer(ARRAY_BUFFER, colorVbo);
	renderer.BufferData(ARRAY_BUFFER, sizeof(float) * 12, colors);
	renderer.VertexAttributePointer(1, 3, 4 * sizeof(float), 0);

	static auto uvVbo = renderer.GenBuffer();
	renderer.BindBuffer(ARRAY_BUFFER, uvVbo);
	renderer.BufferData(ARRAY_BUFFER, sizeof(float) * 6, uvs);
	renderer.VertexAttributePointer(2, 2, 2 * sizeof(float), 0);

	renderer.BindBuffer(ARRAY_BUFFER, 0);
	renderer.BindVertexArray(0);

	renderer.PrintVao(vao);
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
    //RenderBlendPicture2(renderer);
    //RenderTriangleByNearestSampleTexture(renderer);

    //RenderTriangleByBilinearSampleTexture(renderer);

    //RenderTextureRepeatWrap(renderer);
    //RenderTextureMirrorWrap(renderer);

    //RenderTransform1(renderer);
    //RenderTransform2(renderer);
    //RenderTransform3(renderer);
    RenderByBindVAO(renderer);
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
    
    std::cout << "destroyImage..." << std::endl;

    Image::destroyImage(lufei_image);
    Image::destroyImage(goku_image);
    Image::destroyImage(chair_image);

    std::cout << "Application will exit!" << std::endl;

    return 0;
}


// 控制台入口函数
// int main()
// {
//     std::cout << "Hello Dragon Renderer" << std::endl;
//     return 0;
// }