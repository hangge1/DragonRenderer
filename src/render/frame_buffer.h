
#ifndef _RENDER_FRAME_BUFFER_H_
#define _RENDER_FRAME_BUFFER_H_

#include <Windows.h>

#include "frame_buffer.h"

#include "color.h"

class FrameBuffer
{
public:
    FrameBuffer() = default;
    ~FrameBuffer();
    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer& operator=(const FrameBuffer&) = delete;

    void InitFrame(LONG frame_width, LONG frame_height, Color* frame_buffer = nullptr);

    void FillColor(const Color& will_fill_color);

    void SetOnePixelColor(LONG x_pox, LONG y_pox, const Color& color);
private:
    LONG frame_width_ {0};
    LONG frame_height_ {0};
    Color* frame_buffer_ {nullptr};
    bool is_buffer_external {false};
};


#endif