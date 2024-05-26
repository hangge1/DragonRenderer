
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


private:
    static Color lerp_color(const Color& c1, const Color& c2, float weight);
};


#endif