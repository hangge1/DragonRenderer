#ifndef DRAGON_RENDER_CLIP_TOOL_H_
#define DRAGON_RENDER_CLIP_TOOL_H_

#include <iostream>
#include <vector>

#include "pipeline_data.h"

class ClipTool
{
public:
    static void Clip(uint32_t draw_mode, const std::vector<VsOutput>& input_primitive, std::vector<VsOutput>& output);
    static void Clip(uint32_t draw_mode, const std::vector<VsOutput>& input_primitive, std::vector<VsOutput>& output,
        std::vector<VsOutput>& clip_buffer_a, std::vector<VsOutput>& clip_buffer_b);

    // Returns true when the face should be culled.
    static bool CullFace(uint32_t front_face_link_style, uint32_t cull_which_face, const VsOutput& p1, const VsOutput& p2, const VsOutput& p3);

private:
    static void TriangleClip(const glm::vec4& clip_plane, const std::vector<VsOutput>& input, std::vector<VsOutput>& output);
    static void LineClip(const glm::vec4& clip_plane, const std::vector<VsOutput>& input, std::vector<VsOutput>& output);
    static bool IsInSide(const VsOutput& v, const glm::vec4& clip_plane);
    static VsOutput Intersect(const VsOutput& inside_vertex, const VsOutput& outside_vertex, const glm::vec4& clip_plane);
};

#endif
