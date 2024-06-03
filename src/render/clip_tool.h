
#ifndef _RENDER_CLIP_TOOL_H_
#define _RENDER_CLIP_TOOL_H_

#include <iostream>
#include <vector>

#include "pipeline_data.h"

class ClipTool
{
public:
    static void Clip(uint32_t draw_mode, const std::vector<VsOutput>& input_primitive, std::vector<VsOutput>& output);

    //是否剔除，true表示剔除，false表示不剔除
    static bool CullFace(uint32_t front_face_link_style, uint32_t cull_which_face, const VsOutput& p1, const VsOutput& p2, const VsOutput& p3);
private:
    static void TriangleClip(const glm::vec4& clip_plane, std::vector<VsOutput>& clip_result);
    static void LineClip(const glm::vec4& clip_plane, std::vector<VsOutput>& clip_result);
    static bool IsInSide(const VsOutput& v, const glm::vec4 &clip_plane);
    static VsOutput Intersect(const VsOutput& inside_vertex, const VsOutput& outside_vertex, const glm::vec4 &clip_plane);
};

#endif