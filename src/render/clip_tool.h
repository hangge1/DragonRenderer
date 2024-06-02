
#ifndef _RENDER_CLIP_TOOL_H_
#define _RENDER_CLIP_TOOL_H_

#include <iostream>
#include <vector>

#include "pipeline_data.h"

class ClipTool
{
public:
    static void Clip(uint32_t draw_mode, const std::vector<VsOutput>& input_primitive, std::vector<VsOutput>& output);
private:
    static void plane_clip(const glm::vec4& clip_plane, std::vector<VsOutput>& clip_result);
    static bool IsInSide(const VsOutput& v, const glm::vec4 &clip_plane);
    static VsOutput Intersect(const VsOutput& inside_v, const VsOutput& outside_v, const glm::vec4 &clip_plane);
};

#endif