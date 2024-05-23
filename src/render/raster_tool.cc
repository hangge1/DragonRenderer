
#include "raster_tool.h"

#include <iostream>
#include <vector>

#include "pixel.h"

std::vector<Pixel> RasterTool::SimpleRasterizeLine(const Pixel& p1, const Pixel& p2)
{
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;

    std::vector<Pixel> result;
    if(0 == dx)
    {
        for (int y = p1.y; y <= p2.y; y++)
        {
            result.emplace_back(Pixel{p1.x, y});
        }
        return result;
    }
    
    if(0 == dy)
    {
        for (int x = p1.x; x <= p2.x; x++)
        {
            result.emplace_back(Pixel{x, p1.y});
        }
        return result;
    }

    Pixel start {p1};
    Pixel end {p2};

    bool flip_x = false;
    if(dx < 0)
    {
        start.x *= -1;
        end.x *= -1;
        flip_x = true;
    }

    bool flip_y = false;
    if(dy < 0)
    {
        start.y *= -1;
        end.y *= -1;
        flip_y = true;
    }

    //dx != 0 && dy != 0
    double k = (end.y - start.y) / (double)(end.x - start.x);
    double b = start.y - k * start.x;

    bool swap_xy = false;
    if(k - 1.0 > 1e-6)
    {
        swap_xy = true;
    }

    if(swap_xy)
    {
        for (int y = start.y; y <= end.y; y++)
        {
            Pixel need_insert_pixel {(int32_t)((y-b) / k), y};
            if(flip_x)
            {
                need_insert_pixel.x *= -1;
            }
            if(flip_y)
            {
                need_insert_pixel.y *= -1;
            }

            result.emplace_back(need_insert_pixel);
        } 
    }
    else
    {
        for (int x = start.x; x <= end.x; x++)
        {
            Pixel need_insert_pixel {x, (int)(k * x + b)};
            if(flip_x)
            {
                need_insert_pixel.x *= -1;
            }
            if(flip_y)
            {
                need_insert_pixel.y *= -1;
            }

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
    int dy = end.y - start.y;
    int dx = end.x - start.x;

    bool flip_x = false;
    if(dx < 0)
    {
        start.x *= -1;
        end.x *= -1;
        flip_x = true;
    }

    bool flip_y = false;
    if(dy < 0)
    {
        start.y *= -1;
        end.y *= -1;
        flip_y = true;
    }

    dy = end.y - start.y;
    dx = end.x - start.x;

    bool swap_xy = false;
    if(dy > dx)
    {
        std::swap(dx, dy);
        std::swap(start.x, start.y);
        std::swap(end.x, end.y);
        swap_xy = true;
    }

    int pi = dy * 2 - dx;

    int dealtp_NE = 2 * (dy - dx);
    int dealtp_E = 2 * dy;

    int xi = start.x;
    int yi = start.y;

    std::vector<Pixel> result;

    int delta = dx;

    std::cout << "\n\n\n";
    for (int i = 0; i <= delta; i++)
    {
        //std::cout << "[" << i << "] = (" << xi << "," << yi << ")" << std::endl;
        Pixel current_pixel {xi, yi};

        if(swap_xy)
        {
            std::swap(current_pixel.x, current_pixel.y);
        }

        if(flip_x)
        {
            current_pixel.x *= -1;
        }

        if(flip_y)
        {
            current_pixel.y *= -1;
        }

        //颜色插值

        InterpolateLine(p1, p2, current_pixel);

        std::cout << "(" << (int)current_pixel.color.red << "," 
            << (int)current_pixel.color.green << "," << (int)current_pixel.color.blue << ")" << std::endl;

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
    double weight = 1.0f;
    if(start.y != end.y)
    {
        weight = static_cast<double>(target.y - start.y) / (end.y - start.y);
    }
    else
    {
        weight = static_cast<double>(target.x - start.x) / (end.x - start.x);
    }

    target.color.red = static_cast<byte>(weight * static_cast<double>(end.color.red) 
        + (1.0 - weight) * static_cast<double>(start.color.red));
    target.color.green = static_cast<byte>(weight * static_cast<double>(end.color.green) 
        + (1.0 - weight) * static_cast<double>(start.color.green));
    target.color.blue = static_cast<byte>(weight * static_cast<double>(end.color.blue) 
        + (1.0 - weight) * static_cast<double>(start.color.blue));
    target.color.alpha = static_cast<byte>(weight * static_cast<double>(end.color.alpha) 
        + (1.0 - weight) * static_cast<double>(start.color.alpha));
}