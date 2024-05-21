
#ifndef _CORE_PIXEL_H_
#define _CORE_PIXEL_H_

using byte = unsigned char;

class Pixel
{
public:
    Pixel(byte red = 255, byte green = 255, byte blue = 255, byte alpha = 255):
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