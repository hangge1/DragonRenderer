
#ifndef _CORE_COLOR_H_
#define _CORE_COLOR_H_

using byte = unsigned char;

class Color
{
public:
    Color(byte red = 255, byte green = 255, byte blue = 255, byte alpha = 255):
        red_(red),
        green_(green),
        blue_(blue),
        alpha_(alpha)
    {
    }

private:
    byte red_;
    byte green_;
    byte blue_;
    byte alpha_;
};

#endif