
#ifndef _CORE_PIPELINE_DATA_H_
#define _CORE_PIPELINE_DATA_H_

#include "glm/glm.hpp"


#define ARRAY_BUFFER 0
#define ELEMENT_ARRAY_BUFFER 1

#define DRAW_LINES 0
#define DRAW_TRIANGLES 1

#define FRONT_FACE 0
#define BACK_FACE 1
#define FRONT_FACE_CW 0
#define FRONT_FACE_CCW 1

#define CULL_FACE_ENABLE 1

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

    unsigned char b;
    unsigned char g;
    unsigned char r;
    unsigned char a;
};


//仿OpenGL数据结构
struct BindingDescription {
	uint32_t	vbo_id{ 0 };
	size_t		item_size{ 0 };
	size_t		stride{ 0 };
	size_t		offset{ 0 };
};

struct VsOutput 
{
	glm::vec4 position { 0.0f, 0.0f, 0.0f, 1.0f };
	glm::vec4 color; //此处颜色改为0.0-1.0之间表达0-255的量
	glm::vec2 uv;
};

struct FsOutput 
{
	glm::ivec2 pixelPos;
	float depth;
	Color color;//此处使用0-255来进行颜色显示
};

enum TextureUvWrap
{
    WrapRepeat,
    WrapMirror
};



#endif