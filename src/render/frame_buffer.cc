
#include "frame_buffer.h"

#include <iostream>

#include "pixel.h"

FrameBuffer::~FrameBuffer()
{
    if(false == is_buffer_external && nullptr != frame_buffer_)
    {
        delete[] frame_buffer_;
    }
    frame_width_ = 0;
    frame_height_ = 0;
    frame_buffer_ = nullptr;
    is_buffer_external = false;
}

void FrameBuffer::InitFrame(LONG frame_width, LONG frame_height, Pixel* frame_buffer)
{
    frame_width_ = frame_width;
    frame_height_ = frame_height;

    Pixel* temp_frame_buffer {nullptr};
    if(nullptr == frame_buffer)
    {
        is_buffer_external = false;
        temp_frame_buffer = new Pixel[frame_width * frame_height];
    }
    else
    {
        is_buffer_external = true;
        temp_frame_buffer = frame_buffer;
    }

    frame_buffer_ = temp_frame_buffer;
}

void FrameBuffer::FillPixels(const Pixel& will_fill_pixel)
{
    if(nullptr != frame_buffer_)
    {
        std::fill_n(frame_buffer_, frame_width_ * frame_height_, will_fill_pixel);
    }
}

void FrameBuffer::SetOnePixel(LONG x_pox, LONG y_pox, const Pixel& pixel)
{
    if(nullptr == frame_buffer_ || 
        x_pox < 0 || x_pox >= frame_width_ || 
        y_pox < 0 || y_pox >= frame_height_)
    {
        return;
    }

    frame_buffer_[frame_width_ * y_pox + x_pox] = pixel;
}