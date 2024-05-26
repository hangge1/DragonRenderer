
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

void FrameBuffer::InitFrame(LONG frame_width, LONG frame_height, Color* frame_buffer)
{
    frame_width_ = frame_width;
    frame_height_ = frame_height;

    Color* temp_frame_buffer {nullptr};
    if(nullptr == frame_buffer)
    {
        is_buffer_external = false;
        temp_frame_buffer = new Color[frame_width * frame_height];
    }
    else
    {
        is_buffer_external = true;
        temp_frame_buffer = frame_buffer;
    }

    frame_buffer_ = temp_frame_buffer;
}

void FrameBuffer::FillColor(const Color& will_fill_color)
{
    if(nullptr != frame_buffer_)
    {
        std::fill_n(frame_buffer_, frame_width_ * frame_height_, will_fill_color);
    }
}

void FrameBuffer::SetOnePixelColor(LONG x_pox, LONG y_pox, Color& color)
{
    if(nullptr == frame_buffer_ || 
        x_pox < 0 || x_pox >= frame_width_ || 
        y_pox < 0 || y_pox >= frame_height_)
    {
        return;
    }

    frame_buffer_[frame_width_ * y_pox + x_pox] = color;
}

Color* FrameBuffer::GetFrameColor(LONG x_pox, LONG y_pox)
{
    // TODO: 在此处插入 return 语句
    if(nullptr == frame_buffer_ || 
        x_pox < 0 || x_pox >= frame_width_ || 
        y_pox < 0 || y_pox >= frame_height_)
    {
        return nullptr;
    }

    return frame_buffer_ + (frame_width_ * y_pox + x_pox);
}
