
#ifndef _RENDER_FRAME_BUFFER_H_
#define _RENDER_FRAME_BUFFER_H_

#include <Windows.h>

#include "frame_buffer.h"

#include "pipeline_data.h"

class FrameBuffer
{
public:
    FrameBuffer() = default;
    ~FrameBuffer();
    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer& operator=(const FrameBuffer&) = delete;

    void Init(int32_t width, int32_t height, Color* buffer = nullptr);

    void FillColor(const Color& will_fill_color = Color(0,0,0,0));
    void FillDepth(float depth = 1.0f);

    void SetPixelColor(int32_t x_pox, int32_t y_pox, Color& color);
    Color* GetPixelColor(int32_t x_pox, int32_t y_pox) const;

    void SetOnePixelDepth(int32_t x_pox, int32_t y_pox, float depth);
    float GetPixelDepth(int32_t x_pox, int32_t y_pox) const;

    int32_t GetWidth() const { return frame_width_; }
    int32_t GetHeight() const { return frame_height_; }
private:
    int32_t frame_width_ { 0 };
    int32_t frame_height_ { 0 };
    Color* frame_buffer_ { nullptr };
    float* depth_buffer_ { nullptr };
    bool is_buffer_external { false };
};


#endif