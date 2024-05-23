
#ifndef _CORE_COLOR_H_
#define _CORE_COLOR_H_

using byte = unsigned char;

struct Color
{
public:
    Color(byte r = 255, byte g = 255, byte b = 255, byte alp = 255):
        red(r),
        green(g),
        blue(b),
        alpha(alp)
    {
    }
    
    byte blue; //这个定义的顺序不能变,因为bitmap那边的要求就是bgra
    byte green;
    byte red;
    byte alpha;
};

#endif