﻿/*
    Author: 航火火
    Path: main.cpp
    Description:  Application Exe Main Entry Point File
*/

#include <Windows.h>

#include <iostream>

#include "application.h"
#include "renderer.h"
#include "pipeline_data.h"
#include "image.h"
#include "glm/ext.hpp"
#include "shader/default_shader.h"
#include "shader/texture_shader.h"
#include "camera.h"

Camera* camera = nullptr;

//使用的texture
uint32_t texture = 0;
TextureShader* textureShader = nullptr;

uint32_t vao;
uint32_t ebo;

uint32_t vao2;
uint32_t ebo2;

void InitTriangleData(Renderer& renderer)
{
	float positions[] = 
	{	
		-0.5f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,	
		0.5f, -0.5f, 0.0f,
	};

	float colors[] = 
	{
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
	};

	float uvs[] = 
	{
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
	};

	uint32_t indices[] = { 0, 1, 2 };

	//生成indices对应ebo
	ebo = renderer.GenBuffer();
	renderer.BindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
	renderer.BufferData(ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 3, indices);
	renderer.BindBuffer(ELEMENT_ARRAY_BUFFER, 0);

	//生成vao并且绑定
	vao = renderer.GenVertexArray();
	renderer.BindVertexArray(vao);
	
	//生成每个vbo，绑定后，设置属性ID及读取参数
	static auto positionVbo = renderer.GenBuffer();
	renderer.BindBuffer(ARRAY_BUFFER, positionVbo);
	renderer.BufferData(ARRAY_BUFFER, sizeof(float) * 9, positions);
	renderer.VertexAttributePointer(0, 3, 3 * sizeof(float), 0);

	static auto colorVbo = renderer.GenBuffer();
	renderer.BindBuffer(ARRAY_BUFFER, colorVbo);
	renderer.BufferData(ARRAY_BUFFER, sizeof(float) * 12, colors);
	renderer.VertexAttributePointer(1, 4, 4 * sizeof(float), 0);

	static auto uvVbo = renderer.GenBuffer();
	renderer.BindBuffer(ARRAY_BUFFER, uvVbo);
	renderer.BufferData(ARRAY_BUFFER, sizeof(float) * 6, uvs);
	renderer.VertexAttributePointer(2, 2, 2 * sizeof(float), 0);

	renderer.BindBuffer(ARRAY_BUFFER, 0);
	renderer.BindVertexArray(0);

	renderer.PrintVao(vao);
}

void Init2TriangleData(Renderer& renderer)
{
	float positions[] = 
	{	
		-0.5f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,	
		0.5f, -0.5f, 0.0f,

		0.0f, 0.5f, 0.1f,
		0.0f, -0.5f, 0.1f,
		1.0f, -0.5f, 0.1f,
	};

	float colors[] = 
	{
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,

		0.0f, 1.0f, 0.0f, 0.5f,
		0.0f, 1.0f, 0.0f, 0.5f,
		0.0f, 1.0f, 0.0f, 0.5f
	};

	float uvs[] = 
	{
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,

		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f
	};

	renderer.Enable(COLOR_BLEND);

	uint32_t indices[] = { 0, 1, 2, 3, 4, 5 };

	//生成indices对应ebo
	ebo = renderer.GenBuffer();
	renderer.BindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
	renderer.BufferData(ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 6, indices);
	renderer.BindBuffer(ELEMENT_ARRAY_BUFFER, 0);

	//生成vao并且绑定
	vao = renderer.GenVertexArray();
	renderer.BindVertexArray(vao);
	
	//生成每个vbo，绑定后，设置属性ID及读取参数
	static auto positionVbo = renderer.GenBuffer();
	renderer.BindBuffer(ARRAY_BUFFER, positionVbo);
	renderer.BufferData(ARRAY_BUFFER, sizeof(float) * 18, positions);
	renderer.VertexAttributePointer(0, 3, 3 * sizeof(float), 0);

	static auto colorVbo = renderer.GenBuffer();
	renderer.BindBuffer(ARRAY_BUFFER, colorVbo);
	renderer.BufferData(ARRAY_BUFFER, sizeof(float) * 24, colors);
	renderer.VertexAttributePointer(1, 4, 4 * sizeof(float), 0);

	static auto uvVbo = renderer.GenBuffer();
	renderer.BindBuffer(ARRAY_BUFFER, uvVbo);
	renderer.BufferData(ARRAY_BUFFER, sizeof(float) * 12, uvs);
	renderer.VertexAttributePointer(2, 2, 2 * sizeof(float), 0);

	renderer.BindBuffer(ARRAY_BUFFER, 0);
	renderer.BindVertexArray(0);

	renderer.PrintVao(vao);
}

//纹理测试
void Init3TriangleData(Renderer& renderer)
{
	textureShader = new TextureShader();

	static Image* image = Image::createImage(ASSETS_PATH "/texture/goku.jpg");

	texture = renderer.GenTexture();

	renderer.BindTexture(texture);
	renderer.TexImage2D(image->get_width(), image->get_height(), image->get_data());
	renderer.TexParameter(TEXTURE_FILTER, TEXTURE_FILTER_LINEAR);
	renderer.TexParameter(TEXTURE_WRAP_U, TEXTURE_WRAP_REPEAT);
	renderer.TexParameter(TEXTURE_WRAP_V, TEXTURE_WRAP_REPEAT);
	renderer.BindTexture(0);


	renderer.Enable(CULL_FACE);
	renderer.SetFrontFaceLinkStyle(FRONT_FACE_CCW);
	renderer.SetCullWhichFace(BACK_FACE);

	float positions[] = 
	{	
		-0.5f, 0.0f, 0.0f,
		0.5f, 0.0f, 0.0f,	
		0.0f, 0.5f, 0.0f
	};

	float colors[] = 
	{
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f
	};

	float uvs[] = 
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.5f, 1.0f
	};

	//renderer.Enable(COLOR_BLEND);

	uint32_t indices[] = { 0, 1, 2 };

	//生成indices对应ebo
	ebo = renderer.GenBuffer();
	renderer.BindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
	renderer.BufferData(ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 3, indices);
	renderer.BindBuffer(ELEMENT_ARRAY_BUFFER, 0);

	//生成vao并且绑定
	vao = renderer.GenVertexArray();
	renderer.BindVertexArray(vao);
	
	//生成每个vbo，绑定后，设置属性ID及读取参数
	static auto positionVbo = renderer.GenBuffer();
	renderer.BindBuffer(ARRAY_BUFFER, positionVbo);
	renderer.BufferData(ARRAY_BUFFER, sizeof(float) * 9, positions);
	renderer.VertexAttributePointer(0, 3, 3 * sizeof(float), 0);

	static auto colorVbo = renderer.GenBuffer();
	renderer.BindBuffer(ARRAY_BUFFER, colorVbo);
	renderer.BufferData(ARRAY_BUFFER, sizeof(float) * 12, colors);
	renderer.VertexAttributePointer(1, 4, 4 * sizeof(float), 0);

	static auto uvVbo = renderer.GenBuffer();
	renderer.BindBuffer(ARRAY_BUFFER, uvVbo);
	renderer.BufferData(ARRAY_BUFFER, sizeof(float) * 6, uvs);
	renderer.VertexAttributePointer(2, 2, 2 * sizeof(float), 0);

	renderer.BindBuffer(ARRAY_BUFFER, 0);
	renderer.BindVertexArray(0);

	renderer.PrintVao(vao);
}

//利用重构后的仿OpenGL接口，进行渲染
void RenderTriangle(Renderer& renderer)
{
    static auto* shader = new DefaultShader();
    static auto model_matrix = glm::identity<glm::mat4>();

    shader->model_matrix = model_matrix;
    shader->view_matrix = camera->getViewMatrix();
    shader->project_matrix = camera->getProjectionMatrix();

    renderer.UseProgram(shader);
    renderer.BindVertexArray(vao);
    renderer.BindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
    renderer.DrawElement(DRAW_TRIANGLES, 0, 6);
}

void InitLineData(Renderer& renderer)
{
	float positions[] = 
	{
		-0.5f, 0.0f, 0.0f,
		0.5f, 0.0f, 0.0f
	};

	float colors[] = 
	{
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f
	};

	float uvs[] = 
	{
		0.0f, 0.0f,
		0.0f, 1.0f
	};

	uint32_t indices[] = { 0, 1 };

	//生成indices对应ebo
	ebo2 = renderer.GenBuffer();
	renderer.BindBuffer(ELEMENT_ARRAY_BUFFER, ebo2);
	renderer.BufferData(ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 2, indices);
	renderer.BindBuffer(ELEMENT_ARRAY_BUFFER, 0);

	//生成vao并且绑定
	vao2 = renderer.GenVertexArray();
	renderer.BindVertexArray(vao2);
	
	//生成每个vbo，绑定后，设置属性ID及读取参数
	static auto positionVbo = renderer.GenBuffer();
	renderer.BindBuffer(ARRAY_BUFFER, positionVbo);
	renderer.BufferData(ARRAY_BUFFER, sizeof(float) * 6, positions);
	renderer.VertexAttributePointer(0, 3, 3 * sizeof(float), 0);

	static auto colorVbo = renderer.GenBuffer();
	renderer.BindBuffer(ARRAY_BUFFER, colorVbo);
	renderer.BufferData(ARRAY_BUFFER, sizeof(float) * 8, colors);
	renderer.VertexAttributePointer(1, 4, 4 * sizeof(float), 0);

	static auto uvVbo = renderer.GenBuffer();
	renderer.BindBuffer(ARRAY_BUFFER, uvVbo);
	renderer.BufferData(ARRAY_BUFFER, sizeof(float) * 4, uvs);
	renderer.VertexAttributePointer(2, 2, 2 * sizeof(float), 0);

	renderer.BindBuffer(ARRAY_BUFFER, 0);
	renderer.BindVertexArray(0);

	renderer.PrintVao(vao2);
}

//利用重构后的仿OpenGL接口，进行渲染
void RenderLine(Renderer& renderer)
{
    static auto* shader = new DefaultShader();
    static auto model_matrix = glm::identity<glm::mat4>();

    shader->model_matrix = model_matrix;
    shader->view_matrix = camera->getViewMatrix();
    shader->project_matrix = camera->getProjectionMatrix();

    renderer.UseProgram(shader);
    renderer.BindVertexArray(vao2);
    renderer.BindBuffer(ELEMENT_ARRAY_BUFFER, ebo2);
    renderer.DrawElement(DRAW_LINES, 0, 2);
}

void RenderTexture(Renderer& renderer)
{
    static float angle = 0.0f;
    static auto model_matrix = glm::identity<glm::mat4>();

    //angle += 0.01f;
    //model_matrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3{ 0.0f, 1.0f, 0.0f });


    textureShader->model_matrix = model_matrix;
    textureShader->view_matrix = camera->getViewMatrix();
    textureShader->project_matrix = camera->getProjectionMatrix();
	textureShader->diffuse_texture = texture;

    renderer.UseProgram(textureShader);
    renderer.BindVertexArray(vao);
    renderer.BindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
    renderer.DrawElement(DRAW_TRIANGLES, 0, 3);
}

void CustomDraw(Renderer& renderer)
{
    //RenderTriangle(renderer);
    //RenderLine(renderer);
	RenderTexture(renderer);
}

int WINAPI wWinMain(HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPWSTR lpCmdLine,
                    int nCmdShow)
{
    const int window_width = 800;
    const int window_height = 600;
    const char* window_title = "DragonSoftRenderer";

    if(!APP->InitMainWindow(hInstance, window_title, window_width, window_height))
    {
        std::cout << "Application Init Failed!" << std::endl;
        return -1;
    }

	camera = new Camera(glm::radians(60.0f), (float)window_width / (float)window_height, 0.1f, 100.0f, { 0.0f, 1.0f, 0.0f });
	APP->SetCamera(camera);	

    Renderer renderer;
    renderer.InitFrameBuffer(window_width, window_height, APP->GetRenderBuffer());

    //InitTriangleData(renderer);
    //InitLineData(renderer);
	//Init2TriangleData(renderer);
	Init3TriangleData(renderer);
    while(APP->DispatchMessageLoop())
    {
        renderer.Clear();
		camera->update();
        //draw something
        CustomDraw(renderer);
    }
 
    std::cout << "Application will exit!" << std::endl;

	delete camera;
	camera = nullptr;

    return 0;
}
