#include <cstdint>
#include <iostream>
#include <vector>

#include "glm/glm.hpp"

#include "pipeline_data.h"
#include "renderer.h"
#include "shader/default_shader.h"

struct DepthSmokeVertex
{
    float px;
    float py;
    float pz;
    float pw;
    float r;
    float g;
    float b;
    float a;
    float u;
    float v;
};

namespace
{
uint64_t ComputeChecksum(const std::vector<Color>& pixels)
{
    uint64_t checksum = 1469598103934665603ull;
    for(const auto& pixel : pixels)
    {
        checksum ^= pixel.r;
        checksum *= 1099511628211ull;
        checksum ^= pixel.g;
        checksum *= 1099511628211ull;
        checksum ^= pixel.b;
        checksum *= 1099511628211ull;
        checksum ^= pixel.a;
        checksum *= 1099511628211ull;
    }

    return checksum;
}
}

int main()
{
    constexpr int32_t kWidth = 16;
    constexpr int32_t kHeight = 16;

    std::vector<Color> pixels(kWidth * kHeight);

    Renderer renderer;
    renderer.Init(kWidth, kHeight, pixels.data());
    renderer.ClearLayers();
    renderer.BeginFrameStats();
    renderer.Clear();
    renderer.Disable(CULL_FACE);
    renderer.Enable(DEPTH_TEST);
    renderer.SetDepthTestFunc(DEPTH_LESS);

    DefaultShader shader;
    shader.model_matrix = glm::mat4(1.0f);
    shader.view_matrix = glm::mat4(1.0f);
    shader.project_matrix = glm::mat4(1.0f);

    const DepthSmokeVertex vertices[] = {
        {-0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f},
        { 0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f},
        { 0.0f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f, 1.0f},
        {-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f},
        { 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f},
        { 0.0f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.5f, 1.0f},
    };
    const uint32_t indices[] = {3, 4, 5, 0, 1, 2};

    const uint32_t vbo = renderer.GenBuffer();
    renderer.BindBuffer(ARRAY_BUFFER, vbo);
    renderer.BufferData(ARRAY_BUFFER, sizeof(vertices), const_cast<DepthSmokeVertex*>(vertices));

    const uint32_t ebo = renderer.GenBuffer();
    renderer.BindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
    renderer.BufferData(ELEMENT_ARRAY_BUFFER, sizeof(indices), const_cast<uint32_t*>(indices));

    const uint32_t vao = renderer.GenVertexArray();
    renderer.BindVertexArray(vao);
    renderer.BindBuffer(ARRAY_BUFFER, vbo);
    renderer.VertexAttributePointer(0, 4, sizeof(DepthSmokeVertex), offsetof(DepthSmokeVertex, px));
    renderer.VertexAttributePointer(1, 4, sizeof(DepthSmokeVertex), offsetof(DepthSmokeVertex, r));
    renderer.VertexAttributePointer(2, 2, sizeof(DepthSmokeVertex), offsetof(DepthSmokeVertex, u));

    renderer.UseProgram(&shader);
    renderer.BindVertexArray(vao);
    renderer.BindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
    renderer.DrawElement(DRAW_TRIANGLES, 0, 6);

    uint32_t blue_pixels = 0;
    uint32_t green_pixels = 0;
    for(const auto& pixel : pixels)
    {
        if(pixel.r == 0 && pixel.g == 0 && pixel.b == 255 && pixel.a == 255)
        {
            blue_pixels++;
        }
        if(pixel.r == 0 && pixel.g == 255 && pixel.b == 0 && pixel.a == 255)
        {
            green_pixels++;
        }
    }

    const uint64_t checksum = ComputeChecksum(pixels);
    const auto& stats = renderer.GetFrameStats();
    std::cout << "depth output smoke blue_pixels=" << blue_pixels
              << " green_pixels=" << green_pixels
              << " checksum=" << checksum
              << " draw_calls=" << stats.draw_calls
              << " input_triangles=" << stats.input_triangles
              << " rasterized_fragments=" << stats.rasterized_fragments
              << " shaded_fragments=" << stats.shaded_fragments
              << " depth_rejected_fragments=" << stats.depth_rejected_fragments
              << std::endl;

    if(stats.draw_calls != 1 ||
       stats.input_triangles != 2 ||
       stats.rasterized_fragments != 50 ||
       stats.shaded_fragments != 50 ||
       stats.depth_rejected_fragments != 25 ||
       green_pixels != 25 ||
       blue_pixels != 0 ||
       checksum != 689208665967964731ull)
    {
        return 1;
    }

    std::cout << "depth output smoke Pass" << std::endl;
    return 0;
}
