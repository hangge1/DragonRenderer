
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

    static std::vector<Pixel> RasterizeLine(const Pixel& p1, const Pixel& p2);
};


#endif