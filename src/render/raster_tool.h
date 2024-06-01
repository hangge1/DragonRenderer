
#ifndef _RENDER_RASTER_TOOL_H_
#define _RENDER_RASTER_TOOL_H_

#include <vector>

#include "pipeline_data.h"

class RasterTool
{
public:
    RasterTool() = default;
    ~RasterTool() = default;
    RasterTool(const RasterTool&) = delete;
    RasterTool& operator=(const RasterTool&) = delete;

    //新的接口
    static std::vector<VsOutput> Rasterize(uint32_t draw_mode, std::vector<VsOutput>& inputs);
    static void RasterizeLine(std::vector<VsOutput>& result, const VsOutput& p1, const VsOutput& p2);
    static void InterpolateLine(const VsOutput& start, const VsOutput& end, VsOutput& target);
    static void RasterizeTriangle(std::vector<VsOutput>& result, const VsOutput& p1, const VsOutput& p2, const VsOutput& p3);
    static void InterpolateTriangle(const VsOutput& p1, const VsOutput& p2, const VsOutput& p3, VsOutput& target);
};


#endif