#ifndef _RENDER_RENDERER_H
#define _RENDER_RENDERER_H_

#include <Windows.h>

#include "frame_buffer.h"
#include "pixel.h"
#include "image.h"

class Renderer
{
public:
    Renderer() = default;
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void InitFrameBuffer(LONG frame_width, LONG frame_height, void* buffer = nullptr);

    void ClearFrameBuffer();

    void DrawPixel(LONG x_pox, LONG y_pox, Color& pixel_color);

    void DrawPixel(Pixel& point);

    void DrawLine(Pixel& start, Pixel& end);

    void DrawTriangle(Pixel& p1, Pixel& p2, Pixel& p3);

    void DrawPicture(const Image& image);

    void DrawPictureOnBlend(const Image& image, unsigned char src_alpha);

    inline void SetColorBlend(bool start_color_blend) { start_color_blend_ = start_color_blend; }

private:
    Color BlendColor(LONG x, LONG y, const Color& src_color);
private:
    FrameBuffer* current_frame_buffer_ { nullptr };
    bool start_color_blend_  { false };
};


#endif