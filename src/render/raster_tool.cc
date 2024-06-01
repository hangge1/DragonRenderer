
#include "raster_tool.h"

#include <iostream>
#include <vector>

#include "pipeline_data.h"
#include "glm/glm.hpp"


// 计算二维向量的叉乘
float crossProduct(const glm::vec2& a, const glm::vec2& b) 
{
    return a.x * b.y - a.y * b.x;
}

bool IsPointInTriangle(const VsOutput& p1, const VsOutput& p2, const VsOutput& p3, const VsOutput& target)
{
    auto v1 = target.position - p1.position;
    auto v2 = target.position - p2.position;
    auto v3 = target.position - p3.position;

    auto cross1 = crossProduct(v1, v2);
    auto cross2 = crossProduct(v2, v3);
    auto cross3 = crossProduct(v3, v1);

    if(cross1 > 0 && cross2 > 0 && cross3 > 0)
    {
        return true;
    }

    if(cross1 < 0 && cross2 < 0 && cross3 < 0)
    {
        return true;
    }

    return false;
}

std::vector<VsOutput> RasterTool::Rasterize(uint32_t draw_mode, std::vector<VsOutput>& inputs)
{
    std::vector<VsOutput> results;

	if (draw_mode == DRAW_LINES) 
    {
		for (uint32_t i = 0; i < inputs.size(); i += 2) 
        {
			RasterizeLine(results, inputs[i], inputs[i + 1]);
		}
	}
	else if (draw_mode == DRAW_TRIANGLES) 
    {
		for (uint32_t i = 0; i < inputs.size(); i += 3) 
        {
			RasterizeTriangle(results, inputs[i], inputs[i + 1], inputs[i + 2]);
		}
	}

    return results;
}

void RasterTool::RasterizeLine(std::vector<VsOutput>& result, const VsOutput& p1, const VsOutput& p2)
{
    VsOutput start = p1;
    VsOutput end = p2;

    //先假设 p1->p2 第一象限，斜率0 < k < 1

    auto vec_start2end = end.position - start.position;

    bool flip_x = false;
    if(vec_start2end.x < 0)
    {
        start.position.x *= -1;
        end.position.x *= -1;
        flip_x = true;
    }

    bool flip_y = false;
    if(vec_start2end.y < 0)
    {
        start.position.y *= -1;
        end.position.y *= -1;
        flip_y = true;
    }

    vec_start2end = end.position - start.position;

    bool swap_xy = false;
    if(vec_start2end.y > vec_start2end.x)
    {
        std::swap(vec_start2end.x, vec_start2end.y);
        std::swap(start.position.x, start.position.y);
        std::swap(end.position.x, end.position.y);
        swap_xy = true;
    }

    int pi = vec_start2end.y * 2 - vec_start2end.x;

    int dealtp_NE = 2 * (vec_start2end.y - vec_start2end.x);
    int dealtp_E = 2 * vec_start2end.y;

    int xi = start.position.x;
    int yi = start.position.y;

    int delta = vec_start2end.x;

    std::cout << "\n\n\n";
    for (int i = 0; i <= delta; i++)
    {
        VsOutput current_pixel;
        current_pixel.position.x = xi;
        current_pixel.position.y = yi;

        if(swap_xy)
        {
            std::swap(current_pixel.position.x, current_pixel.position.y);
        }

        if(flip_x)
        {
            current_pixel.position.x *= -1;
        }

        if(flip_y)
        {
            current_pixel.position.y *= -1;
        }

        //颜色插值
        InterpolateLine(p1, p2, current_pixel);

        result.emplace_back(current_pixel);
        if(pi > 0)
        {
            yi++;
            pi += dealtp_NE;
        }
        else
        {
            pi += dealtp_E;
        }

        xi++;
    }
}

void RasterTool::InterpolateLine(const VsOutput& start, const VsOutput& end, VsOutput& target)
{
    float weight = 1.0f;

    auto vec_start2target = target.position- start.position;
    auto vec_start2end = end.position - start.position;

    if(start.position.y != end.position.y)
    {
        weight = static_cast<float>(vec_start2target.y) / vec_start2end.y;
    }
    else
    {
        weight = static_cast<float>(vec_start2target.x) / vec_start2end.x;
    }

    target.color.r = weight * end.color.r + (1.0f - weight) * start.color.r;
    target.color.g = weight * end.color.g + (1.0f - weight) * start.color.g;
    target.color.b = weight * end.color.b + (1.0f - weight) * start.color.b;
    target.color.a = weight * end.color.a + (1.0f - weight) * start.color.a;
}

void RasterTool::RasterizeTriangle(std::vector<VsOutput>& result, const VsOutput& p1, const VsOutput& p2, const VsOutput& p3)
{
    int min_x = std::min( std::min( p1.position.x, p2.position.x ), p3.position.x );
    int min_y = std::min( std::min( p1.position.y, p2.position.y ), p3.position.y );
    int max_x = std::max( std::max( p1.position.x, p2.position.x ), p3.position.x );
    int max_y = std::max( std::max( p1.position.y, p2.position.y ), p3.position.y );

    //std::cout << "minx = " << min_x << " maxx = " << max_x
    //          << "miny = " << min_y << " maxy = " << max_y << std::endl;

    for (int x = min_x; x <= max_x; x++)
    {
        for (int y = min_y; y <= max_y; y++)
        {
            VsOutput vs;
            vs.position.x = x;
            vs.position.y = y;

            if(IsPointInTriangle( p1, p2, p3, vs ))
            {
                InterpolateTriangle(p1, p2, p3, vs);
                result.emplace_back(vs);
            }
        }    
    }

    //std::cout << "sum count : " << result.size() << std::endl;
}

void RasterTool::InterpolateTriangle(const VsOutput& p1, const VsOutput& p2, const VsOutput& p3, VsOutput& target)
{
    auto pp1 = p1.position - target.position;
    auto pp2 = p2.position - target.position;
    auto pp3 = p3.position - target.position;

    float s_p12 = std::abs( crossProduct(pp1, pp2) );
    float s_p23 = std::abs( crossProduct(pp2, pp3) );
    float s_p31 = std::abs( crossProduct(pp3, pp1) );

    float s = 1.0f / std::abs( crossProduct(p2.position - p1.position, p3.position - p1.position) );

    //f(p) = coff1 * f(p1) + coff2 * f(p2) + coff3 * f(p3);
    float coff1 = s * s_p23;
    float coff2 = s * s_p31;
    float coff3 = 1.0f - coff2 - coff1; //s * s_p12;

    target.color = p1.color * coff1 + p2.color * coff2 + p3.color * coff3;
    target.uv = p1.uv * coff1 + p2.uv * coff2 + p3.uv * coff3;
}