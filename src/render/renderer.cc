
#include "renderer.h"

#include <iostream>
#include <vector>

#include "pixel.h"
#include "frame_buffer.h"
#include "raster_tool.h"
#include "image.h"

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

void Renderer::DrawPixel(LONG x_pos, LONG y_pos, Color& pixel_color, bool isExChangeRedBlue)
{
    if(isExChangeRedBlue)
    {
        std::swap(pixel_color.r, pixel_color.b);
    }

    current_frame_buffer_->SetOnePixelColor(x_pos, y_pos, pixel_color);
}

void Renderer::DrawPixel(Pixel& point, bool isExChangeRedBlue)
{
    if(isExChangeRedBlue)
    {
        std::swap(point.color.r, point.color.b);
    }
    current_frame_buffer_->SetOnePixelColor(point.pos.x, point.pos.y, point.color);
}

void Renderer::DrawLine(Pixel& start, Pixel& end, bool isExChangeRedBlue)
{
    //auto raster_line = RasterTool::SimpleRasterizeLine(start, end);
    auto raster_line = RasterTool::RasterizeLine(start, end);
    for(auto& p : raster_line)
    {
        DrawPixel(p, isExChangeRedBlue);
    }
}

void Renderer::DrawTriangle(Pixel& p1, Pixel& p2, Pixel& p3, bool isExChangeRedBlue)
{
    auto raster_triangle = RasterTool::RasterizeTriangle(p1, p2, p3);
    for(auto& p : raster_triangle)
    {
        DrawPixel(p, isExChangeRedBlue);
    }
    
}

void Renderer::DrawPicture(const Image& image, bool isExChangeRedBlue)
{
    Color* data = image.get_data();
    if(nullptr == data)
    {
        return;
    }

    uint32_t width = image.get_width();
    uint32_t height = image.get_height();

    for (uint32_t i = 0; i < width; i++)
    {
        for (uint32_t j = 0; j < height; j++)
        {
            DrawPixel(Pixel(i, j, data[j * width + i]), isExChangeRedBlue);
        }
    }
    
}