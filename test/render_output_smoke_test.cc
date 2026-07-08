#include <cstdint>
#include <iostream>
#include <vector>

#include "glm/glm.hpp"

#include "pipeline_data.h"
#include "renderer.h"
#include "shader/default_shader.h"

struct SmokeVertex
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

    DefaultShader shader;
    shader.model_matrix = glm::mat4(1.0f);
    shader.view_matrix = glm::mat4(1.0f);
    shader.project_matrix = glm::mat4(1.0f);

    const SmokeVertex vertices[] = {
        {-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
        { 0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f},
        { 0.0f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.5f, 1.0f},
    };
    const uint32_t indices[] = {0, 1, 2};

    const uint32_t vbo = renderer.GenBuffer();
    renderer.BindBuffer(ARRAY_BUFFER, vbo);
    renderer.BufferData(ARRAY_BUFFER, sizeof(vertices), const_cast<SmokeVertex*>(vertices));

    const uint32_t ebo = renderer.GenBuffer();
    renderer.BindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
    renderer.BufferData(ELEMENT_ARRAY_BUFFER, sizeof(indices), const_cast<uint32_t*>(indices));

    const uint32_t vao = renderer.GenVertexArray();
    renderer.BindVertexArray(vao);
    renderer.BindBuffer(ARRAY_BUFFER, vbo);
    renderer.VertexAttributePointer(0, 4, sizeof(SmokeVertex), offsetof(SmokeVertex, px));
    renderer.VertexAttributePointer(1, 4, sizeof(SmokeVertex), offsetof(SmokeVertex, r));
    renderer.VertexAttributePointer(2, 2, sizeof(SmokeVertex), offsetof(SmokeVertex, u));

    renderer.UseProgram(&shader);
    renderer.BindVertexArray(vao);
    renderer.BindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
    renderer.DrawElement(DRAW_TRIANGLES, 0, 3);

    uint32_t red_pixels = 0;
    uint64_t checksum = 1469598103934665603ull;
    for(const auto& pixel : pixels)
    {
        if(pixel.r == 255 && pixel.g == 0 && pixel.b == 0 && pixel.a == 255)
        {
            red_pixels++;
        }

        checksum ^= pixel.r;
        checksum *= 1099511628211ull;
        checksum ^= pixel.g;
        checksum *= 1099511628211ull;
        checksum ^= pixel.b;
        checksum *= 1099511628211ull;
        checksum ^= pixel.a;
        checksum *= 1099511628211ull;
    }

    const auto& stats = renderer.GetFrameStats();
    std::cout << "render output smoke red_pixels=" << red_pixels
              << " checksum=" << checksum
              << " draw_calls=" << stats.draw_calls
              << " input_triangles=" << stats.input_triangles
              << " rasterized_fragments=" << stats.rasterized_fragments
              << std::endl;

    constexpr uint32_t kExpectedRedPixels = 25;
    constexpr uint64_t kExpectedChecksum = 18130829523236531281ull;
    constexpr uint32_t kExpectedFragments = 25;

    if(stats.draw_calls != 1 || stats.input_triangles != 1)
    {
        return 1;
    }

    if(red_pixels != kExpectedRedPixels ||
        checksum != kExpectedChecksum ||
        stats.rasterized_fragments != kExpectedFragments)
    {
        return 1;
    }

    std::cout << "render output smoke Pass" << std::endl;
    return 0;
}
