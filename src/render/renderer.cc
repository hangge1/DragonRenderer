
#include "renderer.h"

#include <iostream>
#include <vector>

#include "color.h"
#include "frame_buffer.h"
#include "raster_tool.h"

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
    current_frame_buffer_->InitFrame(frame_width, frame_height, (Color*)buffer);
}

void Renderer::ClearFrameBuffer()
{
    if(nullptr != current_frame_buffer_)
    {
        current_frame_buffer_->FillColor(Color(0, 0, 0, 0));
    }
}

void Renderer::DrawPixel(LONG x_pos, LONG y_pos, const Color& pixel_color)
{
    current_frame_buffer_->SetOnePixelColor(x_pos, y_pos, pixel_color);
}

void Renderer::DrawLine(const Pixel& start, const Pixel& end)
{
    auto raster_line = RasterTool::RasterizeLine(start, end);
    //auto raster_line = RasterTool::SimpleRasterizeLine(start, end);
    for(const auto& p : raster_line)
    {
        //std::cout << "(" << p.x << "," << p.y << ")" << std::endl;
        DrawPixel(p.x, p.y, p.color);
    }
}