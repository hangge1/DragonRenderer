
#include "raster_tool.h"

#include <iostream>
#include <vector>

#include "pixel.h"

#include "glm/glm.hpp"

std::vector<Pixel> RasterTool::SimpleRasterizeLine(const Pixel& p1, const Pixel& p2)
{
    auto vec_p1p2 = p2.pos - p1.pos;

    std::vector<Pixel> result;
    if(0 == vec_p1p2.x)
    {
        for (int y = p1.pos.y; y <= p2.pos.y; y++)
        {
            result.emplace_back(Pixel(p1.pos.x, y));
        }
        return result;
    }
    
    if(0 == vec_p1p2.y)
    {
        for (int x = p1.pos.x; x <= p2.pos.x; x++)
        {
            result.emplace_back(Pixel(x, p1.pos.y));
        }
        return result;
    }

    Pixel start { p1 };
    Pixel end { p2 };

    bool flip_x = false;
    if(vec_p1p2.x < 0)
    {
        start.pos.x *= -1;
        end.pos.x *= -1;
        flip_x = true;
    }

    bool flip_y = false;
    if(vec_p1p2.y < 0)
    {
        start.pos.y *= -1;
        end.pos.y *= -1;
        flip_y = true;
    }

    //dx != 0 && dy != 0
    auto vec_start2end = end.pos - start.pos;
    float k = vec_start2end.y / (float)vec_start2end.x;
    float b = start.pos.y - k * start.pos.x;

    bool swap_xy = false;
    if(k - 1.0 > 1e-6)
    {
        swap_xy = true;
    }

    if(swap_xy)
    {
        for (int y = start.pos.y; y <= end.pos.y; y++)
        {
            Pixel need_insert_pixel((y-b) / k, y);
            if(flip_x)
            {
                need_insert_pixel.pos.x *= -1;
            }
            if(flip_y)
            {
                need_insert_pixel.pos.y *= -1;
            }

            InterpolateLine(p1, p2, need_insert_pixel);
            result.emplace_back(need_insert_pixel);
        } 
    }
    else
    {
        for (int x = start.pos.x; x <= end.pos.x; x++)
        {
            Pixel need_insert_pixel(x, k * x + b);
            if(flip_x)
            {
                need_insert_pixel.pos.x *= -1;
            }
            if(flip_y)
            {
                need_insert_pixel.pos.y *= -1;
            }

            InterpolateLine(p1, p2, need_insert_pixel);
            result.emplace_back(need_insert_pixel);
        } 
    }
    
    return result;
}

std::vector<Pixel> RasterTool::RasterizeLine(const Pixel& p1, const Pixel& p2)
{
    Pixel start = p1;
    Pixel end = p2;

    //先假设 p1->p2 第一象限，斜率0 < k < 1
    //暂不考虑颜色
    // int dy = end.pos.y - start.pos.y;
    // int dx = end.pos.x - start.pos.x;

    auto vec_start2end = end.pos - start.pos;

    bool flip_x = false;
    if(vec_start2end.x < 0)
    {
        start.pos.x *= -1;
        end.pos.x *= -1;
        flip_x = true;
    }

    bool flip_y = false;
    if(vec_start2end.y < 0)
    {
        start.pos.y *= -1;
        end.pos.y *= -1;
        flip_y = true;
    }

    // dy = end.pos.y - start.pos.y;
    // dx = end.pos.x - start.pos.x;

    vec_start2end = end.pos - start.pos;

    bool swap_xy = false;
    if(vec_start2end.y > vec_start2end.x)
    {
        std::swap(vec_start2end.x, vec_start2end.y);
        std::swap(start.pos.x, start.pos.y);
        std::swap(end.pos.x, end.pos.y);
        swap_xy = true;
    }

    int pi = vec_start2end.y * 2 - vec_start2end.x;

    int dealtp_NE = 2 * (vec_start2end.y - vec_start2end.x);
    int dealtp_E = 2 * vec_start2end.y;

    int xi = start.pos.x;
    int yi = start.pos.y;

    std::vector<Pixel> result;

    int delta = vec_start2end.x;

    std::cout << "\n\n\n";
    for (int i = 0; i <= delta; i++)
    {
        Pixel current_pixel(xi, yi); 

        if(swap_xy)
        {
            std::swap(current_pixel.pos.x, current_pixel.pos.y);
        }

        if(flip_x)
        {
            current_pixel.pos.x *= -1;
        }

        if(flip_y)
        {
            current_pixel.pos.y *= -1;
        }

        //颜色插值

        InterpolateLine(p1, p2, current_pixel);

        result.emplace_back(current_pixel);
        if(pi > 0)
        {
            yi++;
            pi += dealtp_NE;
        }
        else
        {
            pi += dealtp_E;
        }

        xi++;
    }
 
    return result;
}

void RasterTool::InterpolateLine(const Pixel& start, const Pixel& end, Pixel& target)
{
    float weight = 1.0f;

    auto vec_start2target = target.pos - start.pos;
    auto vec_start2end = end.pos - start.pos;

    if(start.pos.y != end.pos.y)
    {
        weight = static_cast<float>(vec_start2target.y) / vec_start2end.y;
    }
    else
    {
        weight = static_cast<float>(vec_start2target.x) / vec_start2end.x;
    }

    target.color.red = static_cast<byte>(weight * static_cast<float>(end.color.red) 
        + (1.0 - weight) * static_cast<float>(start.color.red));
    target.color.green = static_cast<byte>(weight * static_cast<float>(end.color.green) 
        + (1.0 - weight) * static_cast<float>(start.color.green));
    target.color.blue = static_cast<byte>(weight * static_cast<float>(end.color.blue) 
        + (1.0 - weight) * static_cast<float>(start.color.blue));
    target.color.alpha = static_cast<byte>(weight * static_cast<float>(end.color.alpha) 
        + (1.0 - weight) * static_cast<float>(start.color.alpha));
}

// 计算二维向量的叉乘
float crossProduct(const glm::vec2& a, const glm::vec2& b) 
{
    return a.x * b.y - a.y * b.x;
}


bool IsPointInTriangle(const Pixel& p1, const Pixel& p2, const Pixel& p3, const Pixel& target)
{
    auto v1 = target.pos - p1.pos;
    auto v2 = target.pos - p2.pos;
    auto v3 = target.pos - p3.pos;

    auto cross1 = crossProduct(v1, v2);
    auto cross2 = crossProduct(v2, v3);
    auto cross3 = crossProduct(v3, v1);

    if(cross1 > 0 && cross2 > 0 && cross3 > 0)
    {
        return true;
    }

    if(cross1 < 0 && cross2 < 0 && cross3 < 0)
    {
        return true;
    }

    return false;
}

std::vector<Pixel> RasterTool::RasterizeTriangle(const Pixel& p1, const Pixel& p2, const Pixel& p3)
{
    int min_x = std::min( std::min( p1.pos.x, p2.pos.x ), p3.pos.x );
    int min_y = std::min( std::min( p1.pos.y, p2.pos.y ), p3.pos.y );
    int max_x = std::max( std::max( p1.pos.x, p2.pos.x ), p3.pos.x );
    int max_y = std::max( std::max( p1.pos.y, p2.pos.y ), p3.pos.y );

    //std::cout << "minx = " << min_x << " maxx = " << max_x
    //          << "miny = " << min_y << " maxy = " << max_y << std::endl;

    std::vector<Pixel> result;
    for (int x = min_x; x <= max_x; x++)
    {
        for (int y = min_y; y <= max_y; y++)
        {
            if(IsPointInTriangle( p1, p2, p3, Pixel(x, y) ))
            {
                Pixel temp(x, y); //全白
                InterpolateTriangle(p1, p2, p3, temp);
                result.emplace_back(temp);
            }
        }    
    }

    //std::cout << "sum count : " << result.size() << std::endl;
    
    return result;
}

void RasterTool::InterpolateTriangle(const Pixel& p1, const Pixel& p2, const Pixel& p3, Pixel& target)
{
    auto pp1 = p1.pos - target.pos;
    auto pp2 = p2.pos - target.pos;
    auto pp3 = p3.pos - target.pos;

    float s_p12 = std::abs( crossProduct(pp1, pp2) );
    float s_p23 = std::abs( crossProduct(pp2, pp3) );
    float s_p31 = std::abs( crossProduct(pp3, pp1) );

    float s = 1.0f / std::abs( crossProduct(p2.pos - p1.pos, p3.pos - p1.pos) );

    //f(p) = coff1 * f(p1) + coff2 * f(p2) + coff3 * f(p3);
    float coff1 = s * s_p23;
    float coff2 = s * s_p31;
    float coff3 = 1.0f - coff2 - coff1; //s * s_p12;

    Color target_color;
    target_color.red = coff1 * p1.color.red + coff2 * p2.color.red + coff3 * p3.color.red;
    target_color.green = coff1 * p1.color.green + coff2 * p2.color.green + coff3 * p3.color.green;
    target_color.blue = coff1 * p1.color.blue + coff2 * p2.color.blue + coff3 * p3.color.blue;
    target_color.alpha = coff1 * p1.color.alpha + coff2 * p2.color.alpha + coff3 * p3.color.alpha;

    target.color = target_color;
}