﻿
#ifndef _CORE_PIPELINE_DATA_H_
#define _CORE_PIPELINE_DATA_H_

#include "glm/glm.hpp"


const int ARRAY_BUFFER = 0;
const int ELEMENT_ARRAY_BUFFER = 1;

const int DRAW_LINES = 0;
const int DRAW_TRIANGLES = 1;

const int FRONT_FACE = 0;
const int BACK_FACE = 1;
const int FRONT_FACE_CW = 0;
const int FRONT_FACE_CCW = 1;

const int CULL_FACE = 1;
const int DEPTH_TEST = 2;
const int COLOR_BLEND = 3;


const int DEPTH_LESS = 0;
const int DEPTH_GREATER = 1;

//纹理参数类型选项
const int TEXTURE_FILTER = 0;
const int TEXTURE_WRAP_U = 1;
const int TEXTURE_WRAP_V = 2;

//纹理参数
const int TEXTURE_FILTER_NEAREST = 0;
const int TEXTURE_FILTER_LINEAR = 1;

const int TEXTURE_WRAP_REPEAT = 0;
const int TEXTURE_WRAP_MIRROR = 1;


struct Color
{
    Color(): r(255), g(255), b(255), a(255)
    {

    }

    Color(const Color& other): r(other.r), g(other.g), b(other.b), a(other.a)
    {

    }

    Color(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha):
        r(red), g(green), b(blue), a(alpha)
    {

    }

    glm::u8vec4 operate() const
    {
        return glm::u8vec4(r, g, b, a);
    }

    Color operator*(float k) const
    {
        return Color(r * k, g * k, b * k, a * k);
    }

    Color operator+(const Color& other)
    {
        return Color(r + other.r, g + other.g, b + other.b, a + other.a);
    }

    Color Lerp(const Color& rhs, const float& weight)
    {
        Color result;
        result.r = static_cast<unsigned char>(static_cast<float>(rhs.r) * weight + (1.0f - weight) * static_cast<float>(r));
        result.g = static_cast<unsigned char>(static_cast<float>(rhs.g) * weight + (1.0f - weight) * static_cast<float>(g));
        result.b = static_cast<unsigned char>(static_cast<float>(rhs.b) * weight + (1.0f - weight) * static_cast<float>(b));
        result.a = static_cast<unsigned char>(static_cast<float>(rhs.a) * weight + (1.0f - weight) * static_cast<float>(a));

        return result;
    }

    unsigned char b;
    unsigned char g;
    unsigned char r;
    unsigned char a;
};

//仿OpenGL数据结构
struct BindingDescription 
{
	uint32_t  vbo_id { 0 };
	size_t  item_size { 0 };
	size_t  stride { 0 };
	size_t  offset { 0 };
};

struct VsOutput 
{
    float one_devide_w { 0.0f };
	glm::vec4 position { 0.0f, 0.0f, 0.0f, 1.0f };
	glm::vec4 color; //此处颜色改为0.0-1.0之间表达0-255的量
    glm::vec3 normal;
	glm::vec2 uv;
};

struct FsOutput 
{
	glm::ivec2 pixelPos;
	float depth;
	Color color;//此处使用0-255来进行颜色显示
};



#endif