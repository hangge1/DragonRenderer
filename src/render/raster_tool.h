
#ifndef _RENDER_RASTER_TOOL_H_
#define _RENDER_RASTER_TOOL_H_

#include <vector>

#include "pixel.h"

class RasterTool
{
public:
    RasterTool() = default;
    ~RasterTool() = default;
    RasterTool(const RasterTool&) = delete;
    RasterTool& operator=(const RasterTool&) = delete;

    static std::vector<Pixel> SimpleRasterizeLine(const Pixel& p1, const Pixel& p2);
    static std::vector<Pixel> RasterizeLine(const Pixel& p1, const Pixel& p2);
    static void InterpolateLine(const Pixel& start, const Pixel& end, Pixel& target);

    static std::vector<Pixel> RasterizeTriangle(const Pixel& p1, const Pixel& p2, const Pixel& p3);
    static void InterpolateTriangle(const Pixel& p1, const Pixel& p2, const Pixel& p3, Pixel& target);

    //新的接口
    static std::vector<VsOutput> Rasterize(const uint32_t& draw_mode, std::vector<VsOutput>& inputs);
    static void RasterizeLine(std::vector<VsOutput>& result, const VsOutput& p1, const VsOutput& p2);
    static void InterpolateLine(const VsOutput& start, const VsOutput& end, VsOutput& target);
    static void RasterizeTriangle(std::vector<VsOutput>& result, const VsOutput& p1, const VsOutput& p2, const VsOutput& p3);
    static void InterpolateTriangle(const VsOutput& p1, const VsOutput& p2, const VsOutput& p3, VsOutput& target);
private:
    static Color lerp_color(const Color& c1, const Color& c2, float weight);
};


#endif