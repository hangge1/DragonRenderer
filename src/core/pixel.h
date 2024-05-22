
#ifndef _CORE_PIXEL_H_
#define _CORE_PIXEL_H_

#include <cstdint>

#include "color.h"

struct Pixel
{
    int32_t x;
    int32_t y;
    Color color;
};

#endif