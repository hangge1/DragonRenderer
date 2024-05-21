#ifndef _RENDER_RENDERER_H
#define _RENDER_RENDERER_H_

#include <Windows.h>

#include "pixel.h"
#include "frame_buffer.h"

class Renderer
{
public:
    Renderer() = default;
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void InitFrameBuffer(LONG frame_width, LONG frame_height, void* buffer = nullptr);

    void ClearFrameBuffer();

    void DrawOnePoint(LONG x_pox, LONG y_pox, const Pixel& pixel_point);
private:
    FrameBuffer* current_frame_buffer_ {nullptr};
};


#endif