#include "raster_tool.h"

#include <iostream>
#include <vector>

namespace
{
VsOutput MakePoint(float x, float y)
{
    VsOutput output;
    output.one_devide_w = 1.0f;
    output.position = { x, y, 0.0f, 1.0f };
    output.color = { 1.0f, 1.0f, 1.0f, 1.0f };
    output.normal = { 0.0f, 0.0f, 1.0f };
    output.uv = { 0.0f, 0.0f };
    return output;
}
}

int main()
{
    std::vector<VsOutput> output;

    RasterTool::RasterizeTriangle(
        output,
        MakePoint(-100.0f, -100.0f),
        MakePoint(-50.0f, -100.0f),
        MakePoint(-100.0f, -50.0f),
        8,
        8);
    if(!output.empty())
    {
        std::cout << "raster viewport clamp test Failed" << std::endl;
        return 1;
    }

    RasterTool::RasterizeTriangle(
        output,
        MakePoint(-100.0f, -100.0f),
        MakePoint(200.0f, -100.0f),
        MakePoint(-100.0f, 200.0f),
        8,
        8);
    if(output.size() != 64)
    {
        std::cout << "raster viewport clamp test Failed size=" << output.size() << std::endl;
        return 1;
    }

    std::cout << "raster viewport clamp test Pass" << std::endl;
    return 0;
}
