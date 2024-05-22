
#include "raster_tool.h"

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

    //dx != 0 && dy != 0
    double k = (p2.y - p1.y) / (double)(p2.x - p1.x);
    double b = p1.y - k * p1.x;

    for (int x = p1.x; x <= p2.x; x++)
    {
        result.emplace_back(Pixel{x, (int)(k * x + b)});
    }      

    return result;
}

std::vector<Pixel> RasterTool::RasterizeLine(const Pixel& p1, const Pixel& p2)
{
    //TODO

    Pixel start = p1;
    Pixel end = p2;
    
    //先假设 p1->p2 第一象限，斜率0 < k < 1
    //暂不考虑颜色
    int dy = end.y - start.y;
    int dx = end.x - start.x;
    int pi = dy * 2 - dx;

    int dealtp_NE = 2 * (dy - dx);
    int dealtp_E = 2 * dy;

    int xi = start.x;
    int yi = start.y;
    std::vector<Pixel> result;

    for (int i = start.x; i <= end.x; i++)
    {
        result.emplace_back(Pixel{i, yi});
        if(pi > 0)
        {
            yi++;
            pi += dealtp_NE;
        }
        else
        {
            pi += dealtp_E;
        }
    }

    return result;
}