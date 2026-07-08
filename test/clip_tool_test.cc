#include <cmath>
#include <iostream>
#include <vector>

#include "clip_tool.h"
#include "pipeline_data.h"

namespace
{
VsOutput MakeVertex(float x, float y, float z, float w = 1.0f)
{
    VsOutput output;
    output.position = {x, y, z, w};
    output.color = {1.0f, 1.0f, 1.0f, 1.0f};
    output.uv = {0.0f, 0.0f};
    return output;
}

bool IsInsideClipVolume(const VsOutput& output)
{
    const auto& p = output.position;
    constexpr float kEpsilon = 1e-5f;
    return p.w >= -kEpsilon &&
        p.x <= p.w + kEpsilon &&
        p.x >= -p.w - kEpsilon &&
        p.y <= p.w + kEpsilon &&
        p.y >= -p.w - kEpsilon &&
        p.z <= p.w + kEpsilon &&
        p.z >= -p.w - kEpsilon;
}

bool EveryVertexInside(const std::vector<VsOutput>& outputs)
{
    for(const auto& output : outputs)
    {
        if(!IsInsideClipVolume(output))
        {
            return false;
        }
    }

    return true;
}

bool Expect(bool condition, const char* message)
{
    if(!condition)
    {
        std::cout << "clip tool test failed: " << message << std::endl;
        return false;
    }

    return true;
}
}

int main()
{
    std::vector<VsOutput> output;

    const std::vector<VsOutput> inside_triangle = {
        MakeVertex(-0.25f, -0.25f, 0.0f),
        MakeVertex(0.25f, -0.25f, 0.0f),
        MakeVertex(0.0f, 0.25f, 0.0f),
    };
    ClipTool::Clip(DRAW_TRIANGLES, inside_triangle, output);
    if(!Expect(output.size() == 3, "inside triangle should stay one triangle") ||
        !Expect(EveryVertexInside(output), "inside triangle output should stay inside clip volume"))
    {
        return 1;
    }

    const std::vector<VsOutput> outside_triangle = {
        MakeVertex(1.5f, -0.25f, 0.0f),
        MakeVertex(1.5f, 0.25f, 0.0f),
        MakeVertex(1.8f, 0.0f, 0.0f),
    };
    ClipTool::Clip(DRAW_TRIANGLES, outside_triangle, output);
    if(!Expect(output.empty(), "fully outside triangle should be clipped away"))
    {
        return 1;
    }

    const std::vector<VsOutput> crossing_triangle = {
        MakeVertex(0.5f, -0.5f, 0.0f),
        MakeVertex(1.5f, 0.0f, 0.0f),
        MakeVertex(0.5f, 0.5f, 0.0f),
    };
    ClipTool::Clip(DRAW_TRIANGLES, crossing_triangle, output);
    if(!Expect(output.size() == 6, "right-plane crossing triangle should produce two triangles") ||
        !Expect(EveryVertexInside(output), "clipped crossing triangle should be inside clip volume"))
    {
        return 1;
    }

    const std::vector<VsOutput> crossing_line = {
        MakeVertex(-1.5f, 0.0f, 0.0f),
        MakeVertex(0.0f, 0.0f, 0.0f),
    };
    ClipTool::Clip(DRAW_LINES, crossing_line, output);
    if(!Expect(output.size() == 2, "left-plane crossing line should keep one clipped segment") ||
        !Expect(EveryVertexInside(output), "clipped line should be inside clip volume"))
    {
        return 1;
    }

    const VsOutput p1 = MakeVertex(0.0f, 0.0f, 0.0f);
    const VsOutput p2 = MakeVertex(1.0f, 0.0f, 0.0f);
    const VsOutput p3 = MakeVertex(0.0f, 1.0f, 0.0f);
    if(!Expect(!ClipTool::CullFace(FRONT_FACE_CCW, BACK_FACE, p1, p2, p3),
            "CCW front with back-face culling should keep CCW triangle") ||
        !Expect(ClipTool::CullFace(FRONT_FACE_CCW, FRONT_FACE, p1, p2, p3),
            "CCW front with front-face culling should cull CCW triangle") ||
        !Expect(ClipTool::CullFace(FRONT_FACE_CW, BACK_FACE, p1, p2, p3),
            "CW front with back-face culling should cull CCW triangle") ||
        !Expect(!ClipTool::CullFace(FRONT_FACE_CW, FRONT_FACE, p1, p2, p3),
            "CW front with front-face culling should keep CCW triangle"))
    {
        return 1;
    }

    std::cout << "clip tool test Pass" << std::endl;
    return 0;
}
