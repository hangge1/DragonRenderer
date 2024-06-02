#include "clip_tool.h"



void ClipTool::Clip(uint32_t draw_mode, const std::vector<VsOutput> &input_primitive, std::vector<VsOutput> &output)
{
    std::vector<glm::vec4> clip_planes = 
    {
        {0,0,0,1},
        {-1,0,0,1},
        {1,0,0,1},
        {0,-1,0,1},
        {0,1,0,1},
        {0,0,-1,1},
        {0,0,1,1}
    };

    output.clear();

    if(draw_mode == DRAW_TRIANGLES)
    {
        for (int i = 0; i < input_primitive.size(); i += 3)
        {
            //获取当前三角形
            if(i + 2 >= input_primitive.size())
            {
                break;
            }

            //初始化为三角形的三个顶点
            std::vector<VsOutput> current_tri_clip_result 
            { 
                input_primitive[i], 
                input_primitive[i + 1], 
                input_primitive[i + 2] 
            };

            //逐面裁剪三角形
            for (int j = 0; j < clip_planes.size(); j++)
            {
                TriangleClip(clip_planes[j], current_tri_clip_result);
            }

            if(current_tri_clip_result.empty())
            {
                continue;
            }

            //三角形重建
            std::vector<VsOutput> current_tri_final_result;
            for (int k = 1; k < current_tri_clip_result.size() - 1; k++)
            {
                current_tri_final_result.emplace_back(current_tri_clip_result[0]);
                current_tri_final_result.emplace_back(current_tri_clip_result[k]);
                current_tri_final_result.emplace_back(current_tri_clip_result[k + 1]);
            }

            //加入结果集
            for(const auto& v : current_tri_final_result)
            {
                output.emplace_back(v);
            }
        }
    }
    else if(draw_mode == DRAW_LINES)
    {
        for (int i = 0; i < input_primitive.size(); i += 2)
        {
            //获取当前直线
            if(i + 1 >= input_primitive.size())
            {
                break;
            }

            //初始化为直线的2个顶点
            std::vector<VsOutput> current_line_clip_result { input_primitive[i], input_primitive[i + 1] };

            //逐面裁剪直线
            for (int j = 0; j < clip_planes.size(); j++)
            {
                LineClip(clip_planes[j], current_line_clip_result);
            }

            if(current_line_clip_result.empty())
            {
                continue;
            }

            //加入结果集
            for(const auto& v : current_line_clip_result)
            {
                output.emplace_back(v);
            }
        }
    }
}

void ClipTool::TriangleClip(const glm::vec4 &clip_plane, std::vector<VsOutput> &clip_result)
{
    std::vector<VsOutput> targetVertexList = clip_result;

    clip_result.clear();

    for (int i = 0; i < targetVertexList.size(); i++)
    {
        auto last_vertex = targetVertexList[i];
        auto current_vertex = targetVertexList[(i + 1) % targetVertexList.size()];

        if(IsInSide(last_vertex, clip_plane))
        {
            //（1）last内侧,current在内侧 => 添加current
            if(IsInSide(current_vertex, clip_plane))
            {
                clip_result.emplace_back(current_vertex);
            }
            else //（2）last内侧,current在外侧 => 计算交点，添加交点
            {
                clip_result.emplace_back(Intersect(last_vertex, current_vertex, clip_plane));
            }          
        }
        else
        {
            //(3) last外侧, current 在内侧 => 添加交点和current
            if(IsInSide(current_vertex, clip_plane))
            {
                //计算交点，添加交点
                clip_result.emplace_back(Intersect(current_vertex, last_vertex, clip_plane));
                //添加current
                clip_result.emplace_back(current_vertex);
            }
            //(4) last外侧, current 外侧 => 不添加
        }
    }
}

void ClipTool::LineClip(const glm::vec4& clip_plane, std::vector<VsOutput>& clip_result)
{
    std::vector<VsOutput> targetVertexList = clip_result;

    clip_result.clear();

    if(targetVertexList.size() < 2)
    {
        return;
    }

    auto last_vertex = targetVertexList[0];
    auto current_vertex = targetVertexList[1];

    if(IsInSide(last_vertex, clip_plane))
    {
        //（1）last内侧,current在内侧 => 先添加last，再current
        if(IsInSide(current_vertex, clip_plane))
        {
            clip_result.emplace_back(last_vertex);
            clip_result.emplace_back(current_vertex);
        }
        else //（2）last内侧,current在外侧 => 计算交点，先添加last，再添加交点
        {
            clip_result.emplace_back(last_vertex);
            clip_result.emplace_back(Intersect(last_vertex, current_vertex, clip_plane));
        }          
    }
    else
    {
        //(3) last外侧, current 在内侧 => 添加交点和current
        if(IsInSide(current_vertex, clip_plane))
        {
            //计算交点，添加交点
            clip_result.emplace_back(Intersect(current_vertex, last_vertex, clip_plane));
            //添加current
            clip_result.emplace_back(current_vertex);
        }
        //(4) last外侧, current 外侧 => 不添加
    }
}


bool ClipTool::IsInSide(const VsOutput& v, const glm::vec4 &clip_plane)
{
    return glm::dot(v.position, clip_plane) >= 0.0f; 
}

VsOutput ClipTool::Intersect(const VsOutput& inside_v, const VsOutput& outside_v, const glm::vec4 &clip_plane)
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