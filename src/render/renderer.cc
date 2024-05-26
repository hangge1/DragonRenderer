
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

void Renderer::DrawPixel(LONG x_pos, LONG y_pos, Color& pixel_color)
{
    current_frame_buffer_->SetOnePixelColor(x_pos, y_pos, BlendColor(x_pos, y_pos, pixel_color));
}

void Renderer::DrawPixel(Pixel& point)
{
    current_frame_buffer_->SetOnePixelColor(point.pos.x, point.pos.y, BlendColor(point.pos.x, point.pos.x, point.color));
}

void Renderer::DrawLine(Pixel& start, Pixel& end)
{
    //auto raster_line = RasterTool::SimpleRasterizeLine(start, end);
    auto raster_line = RasterTool::RasterizeLine(start, end);
    for(auto& p : raster_line)
    {
        DrawPixel(p);
    }
}

void Renderer::DrawTriangle(Pixel& p1, Pixel& p2, Pixel& p3)
{
    auto raster_triangle = RasterTool::RasterizeTriangle(p1, p2, p3);

    if(texture_)
    {
        for(auto& p : raster_triangle)
        {
            p.color = NearestUvSample(p.uv);
            DrawPixel(p);
        }
    }
    else
    {
        for(auto& p : raster_triangle)
        {
            DrawPixel(p);
        }
    }   
}

void Renderer::DrawPicture(const Image& image)
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
            Color blend_color = BlendColor(i, j, data[j * width + i]);
            DrawPixel(Pixel(i, j, blend_color));
        }
    }   
}

void Renderer::DrawPictureOnBlend(const Image& image, unsigned char src_alpha)
{
    Color* data = image.get_data();
    if(nullptr == data)
    {
        return;
    }

    if(src_alpha < 0)
    {
        src_alpha = 0;
    }

    if(src_alpha > 255)
    {
        src_alpha = 255;
    }

    uint32_t width = image.get_width();
    uint32_t height = image.get_height();

    SetColorBlend(true);
    for (uint32_t i = 0; i < width; i++)
    {
        for (uint32_t j = 0; j < height; j++)
        {
            Color src_color(data[j * width + i]);
            src_color.a = src_alpha;
            Color blend_color = BlendColor(i, j, src_color);
            DrawPixel(Pixel(i, j, blend_color));
        }
    } 
    SetColorBlend(false);
}

Color Renderer::BlendColor(LONG x, LONG y, const Color& src_color)
{
    if(nullptr == current_frame_buffer_)
    {
        return src_color;
    }

    Color* dst_color = current_frame_buffer_->GetFrameColor(x, y);
    if(nullptr == dst_color)
    {
        return src_color;
    }


    if(start_color_blend_)
    {
        float src_weight = src_color.a / 255.0f;

        Color result;
        result.r = src_color.r * src_weight + dst_color->r * (1.0f - src_weight);
        result.g = src_color.g * src_weight + dst_color->g * (1.0f - src_weight);
        result.b = src_color.b * src_weight + dst_color->b * (1.0f - src_weight);

        return result;
    }

    return src_color;
}

Color Renderer::NearestUvSample(const glm::vec2 &uv)
{
    if(nullptr == texture_ || nullptr == current_frame_buffer_)
    {
        return Color();
    }

    int x = uv.x * (texture_->get_width() - 1);
    int y = uv.y * (texture_->get_height() - 1);

    return texture_->get_data()[y * texture_->get_width() + x];
}
