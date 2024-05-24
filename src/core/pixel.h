
#ifndef _CORE_PIXEL_H_
#define _CORE_PIXEL_H_

#include "glm/glm.hpp"

using Color = glm::u8vec4;

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