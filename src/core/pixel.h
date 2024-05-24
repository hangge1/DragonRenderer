
#ifndef _CORE_PIXEL_H_
#define _CORE_PIXEL_H_

#include <cstdint>

#include "color.h"

#include "glm/glm.hpp"

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