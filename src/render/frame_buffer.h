
#ifndef _RENDER_FRAME_BUFFER_H_
#define _RENDER_FRAME_BUFFER_H_

#include <Windows.h>

#include "frame_buffer.h"

#include "pixel.h"

class FrameBuffer
{
public:
    FrameBuffer() = default;
    ~FrameBuffer();
    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer& operator=(const FrameBuffer&) = delete;

    void InitFrame(LONG frame_width, LONG frame_height, Pixel* frame_buffer = nullptr);

    void FillPixels(const Pixel& will_fill_pixel);

    void SetOnePixel(LONG x_pox, LONG y_pox, const Pixel& pixel);
private:
    LONG frame_width_ {0};
    LONG frame_height_ {0};
    Pixel* frame_buffer_ {nullptr};
    bool is_buffer_external {false};
};


#endif