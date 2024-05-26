
#ifndef _CORE_PIXEL_H_
#define _CORE_PIXEL_H_

#include "glm/glm.hpp"

//using Color = glm::u8vec4;

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



struct Pixel
{
    Pixel(int x = 0, int y = 0, Color r = Color(), float u = 0.0f, float v = 0.0f)
        : pos(x, y), 
          color(r),
          uv(u, v)
    {

    }

    glm::ivec2 pos;
    Color color;
    glm::vec2 uv;
};

#endif