
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

    unsigned char b;
    unsigned char g;
    unsigned char r;
    unsigned char a;
};

struct Pixel
{
    Pixel(int x = 0, int y = 0, Color r = Color())
        : pos(x, y), 
          color(r)
    {

    }

    glm::ivec2 pos;
    Color color;
};

#endif