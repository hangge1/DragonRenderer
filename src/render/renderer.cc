
#include "renderer.h"

#include "pixel.h"
#include "frame_buffer.h"

Renderer::~Renderer()
{
    if(nullptr != current_frame_buffer_)
    {
        delete current_frame_buffer_;
    }
}

void Renderer::InitFrameBuffer(LONG frame_width, LONG frame_height, void* buffer)
{
    if(nullptr != current_frame_buffer_)
    {
        delete current_frame_buffer_;
    }

    current_frame_buffer_ = new FrameBuffer();
    current_frame_buffer_->InitFrame(frame_width, frame_height, (Pixel*)buffer);
}

void Renderer::ClearFrameBuffer()
{
    if(nullptr != current_frame_buffer_)
    {
        current_frame_buffer_->FillPixels(Pixel(0, 0, 0, 0));
    }
}

void Renderer::DrawOnePoint(LONG x_pos, LONG y_pos, const Pixel& pixel_point)
{
    current_frame_buffer_->SetOnePixel(x_pos, y_pos, pixel_point);
}