#include "raster_tool.h"

#include <algorithm>
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

    std::vector<VsOutput> inputs = {
        MakePoint(-100.0f, -100.0f),
        MakePoint(200.0f, -100.0f),
        MakePoint(-100.0f, 200.0f)
    };
    size_t flushed_fragments = 0;
    size_t max_chunk_size = 0;
    size_t flush_count = 0;
    RasterTool::RasterizeChunked(
        DRAW_TRIANGLES,
        inputs,
        output,
        8,
        8,
        10,
        [&](std::vector<VsOutput>& chunk) {
            flushed_fragments += chunk.size();
            max_chunk_size = std::max(max_chunk_size, chunk.size());
            flush_count++;
        });
    if(flushed_fragments != 64 || max_chunk_size > 10 || flush_count < 2 || !output.empty())
    {
        std::cout << "raster viewport clamp test Failed chunked flushed=" << flushed_fragments
            << " max_chunk=" << max_chunk_size
            << " flush_count=" << flush_count
            << " remaining=" << output.size() << std::endl;
        return 1;
    }

    std::cout << "raster viewport clamp test Pass" << std::endl;
    return 0;
}
