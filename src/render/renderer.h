#ifndef _RENDER_RENDERER_H
#define _RENDER_RENDERER_H_

#include <Windows.h>

#include "color.h"
#include "frame_buffer.h"
#include "pixel.h"

class Renderer
{
public:
    Renderer() = default;
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void InitFrameBuffer(LONG frame_width, LONG frame_height, void* buffer = nullptr);

    void ClearFrameBuffer();

    void DrawPixel(LONG x_pox, LONG y_pox, const Color& pixel_color);

    void DrawPixel(const Pixel& point);

    void DrawLine(const Pixel& start, const Pixel& end);
private:
    FrameBuffer* current_frame_buffer_ {nullptr};
};


#endif