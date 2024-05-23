
#include <iostream>
#include <vector>
#include <sstream>
#include <string>

#include "raster_tool.h"
#include "pixel.h"

bool Str2Int(const char* str, int& num)
{
    std::stringstream ss(str);
    ss >> num;
    return !ss.fail();
}

std::string StrPixel(const Pixel& pixel)
{
    std::stringstream ss;
    ss << "(" << pixel.x << "," << pixel.y << ")";
    return ss.str();
}

void PrintLine(const std::vector<Pixel>& brensenham_line, const std::vector<Pixel>& sample_line)
{
    auto size = std::min(brensenham_line.size(), sample_line.size());

    std::cout << "index\tbrensemham\tsample" << std::endl;
    for (int i = 0; i < size; i++)
    {
        std::cout << i << "\t" << StrPixel(brensenham_line[i]) << "\t\t" << StrPixel(sample_line[i]) << std::endl;
    }

    if(size < brensenham_line.size())
    {
        //sample少
        for (int i = size; i < brensenham_line.size(); i++)
        {
            std::cout << i << "\t" << StrPixel(brensenham_line[i]) << "\t\t" << std::endl;
        }
    }
    else
    {
        //brensenham少
        for (int i = size; i < sample_line.size(); i++)
        {
            std::cout << i << "\t\t\t" << StrPixel(sample_line[i]) << std::endl;
        }
    }
}

int main(int argc, char** argv)
{
    //./RasterizeLineTest.exe 100 100 200 200
    //start = (100, 100)
    //end = (200, 200)
    if(argc < 5)
    {
        std::cout << "Usage: ./RasterizeLineTest.exe p1.x p1.y p2.x p2.y" << std::endl;
        return -1;
    }

    Pixel p1;
    Pixel p2;

    if(!Str2Int(argv[1], p1.x))
    {
        std::cout << "p1.x convert to int failed" << std::endl;
        return -2;
    }
    if(!Str2Int(argv[2], p1.y))
    {
        std::cout << "p1.y convert to int failed" << std::endl;
        return -2;
    }
    if(!Str2Int(argv[3], p2.x))
    {
        std::cout << "p2.x convert to int failed" << std::endl;
        return -2;
    }
    if(!Str2Int(argv[4], p2.y))
    {
        std::cout << "p2.y convert to int failed" << std::endl;
        return -2;
    }

    std::vector<Pixel> brensenham_line = RasterTool::RasterizeLine(p1, p2);
    std::vector<Pixel> line = RasterTool::SimpleRasterizeLine(p1, p2);

    if(line.size() != brensenham_line.size())
    {
        std::cout << "line count is not same " << std::endl;
        std::cout << "simple count: " << line.size() << " brensenham count: " << brensenham_line.size() << std::endl;
        PrintLine(brensenham_line, line);
        return -3;
    }

    for (int i = 0; i < line.size(); i++)
    {
        auto& p = line[i];
        auto& bren_p = brensenham_line[i];

        if(p.x != bren_p.x || p.y != bren_p.y)
        {
            std::cout << "point is not completely at same position" << std::endl;
            PrintLine(brensenham_line, line);
            return -4;
        }
    }
    
    std::cout << "line_rasterize_test Test Pass" << std::endl;
    return 0;
}