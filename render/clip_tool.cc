#include "clip_tool.h"

#include <array>

void ClipTool::Clip(uint32_t draw_mode, const std::vector<VsOutput>& input_primitive, std::vector<VsOutput>& output)
{
    std::vector<VsOutput> clip_buffer_a;
    std::vector<VsOutput> clip_buffer_b;
    Clip(draw_mode, input_primitive, output, clip_buffer_a, clip_buffer_b);
}

void ClipTool::Clip(uint32_t draw_mode, const std::vector<VsOutput>& input_primitive, std::vector<VsOutput>& output,
    std::vector<VsOutput>& clip_buffer_a, std::vector<VsOutput>& clip_buffer_b)
{
    static constexpr std::array<glm::vec4, 7> clip_planes =
    {
        glm::vec4{0, 0, 0, 1},
        glm::vec4{-1, 0, 0, 1},
        glm::vec4{1, 0, 0, 1},
        glm::vec4{0, -1, 0, 1},
        glm::vec4{0, 1, 0, 1},
        glm::vec4{0, 0, -1, 1},
        glm::vec4{0, 0, 1, 1}
    };

    output.clear();
    output.reserve(input_primitive.size());

    if(draw_mode == DRAW_TRIANGLES)
    {
        clip_buffer_a.reserve(12);
        clip_buffer_b.reserve(12);

        for(size_t i = 0; i < input_primitive.size(); i += 3)
        {
            if(i + 2 >= input_primitive.size())
            {
                break;
            }

            clip_buffer_a.clear();
            clip_buffer_b.clear();
            clip_buffer_a.emplace_back(input_primitive[i]);
            clip_buffer_a.emplace_back(input_primitive[i + 1]);
            clip_buffer_a.emplace_back(input_primitive[i + 2]);

            auto* input = &clip_buffer_a;
            auto* clipped = &clip_buffer_b;

            for(const auto& clip_plane : clip_planes)
            {
                TriangleClip(clip_plane, *input, *clipped);
                std::swap(input, clipped);

                if(input->empty())
                {
                    break;
                }
            }

            if(input->empty())
            {
                continue;
            }

            for(size_t k = 1; k + 1 < input->size(); k++)
            {
                output.emplace_back((*input)[0]);
                output.emplace_back((*input)[k]);
                output.emplace_back((*input)[k + 1]);
            }
        }
    }
    else if(draw_mode == DRAW_LINES)
    {
        clip_buffer_a.reserve(2);
        clip_buffer_b.reserve(2);

        for(size_t i = 0; i < input_primitive.size(); i += 2)
        {
            if(i + 1 >= input_primitive.size())
            {
                break;
            }

            clip_buffer_a.clear();
            clip_buffer_b.clear();
            clip_buffer_a.emplace_back(input_primitive[i]);
            clip_buffer_a.emplace_back(input_primitive[i + 1]);

            auto* input = &clip_buffer_a;
            auto* clipped = &clip_buffer_b;

            for(const auto& clip_plane : clip_planes)
            {
                LineClip(clip_plane, *input, *clipped);
                std::swap(input, clipped);

                if(input->empty())
                {
                    break;
                }
            }

            for(const auto& v : *input)
            {
                output.emplace_back(v);
            }
        }
    }
}

bool ClipTool::CullFace(uint32_t front_face_link_style, uint32_t cull_which_face, 
                        const VsOutput& p1, const VsOutput& p2, const VsOutput& p3)
{
    glm::vec3 v1 = p2.position - p1.position;
    glm::vec3 v2 = p3.position - p1.position;

    glm::vec3 cross = glm::cross(v1, v2);

    if(cull_which_face == FRONT_FACE)
    {
        if(front_face_link_style == FRONT_FACE_CW)
        {
            return cross.z < 0;
        }

        return cross.z > 0;
    }

    if(front_face_link_style == FRONT_FACE_CW)
    {
        return cross.z > 0;
    }

    return cross.z < 0;
}

void ClipTool::TriangleClip(const glm::vec4& clip_plane, const std::vector<VsOutput>& input, std::vector<VsOutput>& output)
{
    output.clear();
    output.reserve(input.size() + 1);

    for(size_t i = 0; i < input.size(); i++)
    {
        const auto& last_vertex = input[i];
        const auto& current_vertex = input[(i + 1) % input.size()];
        const bool last_inside = IsInSide(last_vertex, clip_plane);
        const bool current_inside = IsInSide(current_vertex, clip_plane);

        if(last_inside)
        {
            if(current_inside)
            {
                output.emplace_back(current_vertex);
            }
            else
            {
                output.emplace_back(Intersect(last_vertex, current_vertex, clip_plane));
            }
        }
        else
        {
            if(current_inside)
            {
                output.emplace_back(Intersect(current_vertex, last_vertex, clip_plane));
                output.emplace_back(current_vertex);
            }
        }
    }
}

void ClipTool::LineClip(const glm::vec4& clip_plane, const std::vector<VsOutput>& input, std::vector<VsOutput>& output)
{
    output.clear();
    output.reserve(2);

    if(input.size() < 2)
    {
        return;
    }

    const auto& last_vertex = input[0];
    const auto& current_vertex = input[1];
    const bool last_inside = IsInSide(last_vertex, clip_plane);
    const bool current_inside = IsInSide(current_vertex, clip_plane);

    if(last_inside)
    {
        if(current_inside)
        {
            output.emplace_back(last_vertex);
            output.emplace_back(current_vertex);
        }
        else
        {
            output.emplace_back(last_vertex);
            output.emplace_back(Intersect(last_vertex, current_vertex, clip_plane));
        }
    }
    else
    {
        if(current_inside)
        {
            output.emplace_back(Intersect(current_vertex, last_vertex, clip_plane));
            output.emplace_back(current_vertex);
        }
    }
}

bool ClipTool::IsInSide(const VsOutput& v, const glm::vec4& clip_plane)
{
    return glm::dot(v.position, clip_plane) >= 0.0f;
}

VsOutput ClipTool::Intersect(const VsOutput& inside_v, const VsOutput& outside_v, const glm::vec4& clip_plane)
{
    VsOutput result;

    float out_distance = glm::dot(outside_v.position, clip_plane);
    float in_distance = glm::dot(inside_v.position, clip_plane);

    float weight = out_distance / (out_distance - in_distance);

    result.position = glm::mix(outside_v.position, inside_v.position, weight);
    result.color = glm::mix(outside_v.color, inside_v.color, weight);
    result.uv = glm::mix(outside_v.uv, inside_v.uv, weight);

    return result;
}
