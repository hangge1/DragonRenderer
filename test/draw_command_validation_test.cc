#include <cstdint>
#include <iostream>
#include <vector>

#include "glm/glm.hpp"

#include "pipeline_data.h"
#include "renderer.h"
#include "shader/default_shader.h"

struct ValidationVertex
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
    constexpr int32_t kWidth = 8;
    constexpr int32_t kHeight = 8;

    std::vector<Color> pixels(kWidth * kHeight);

    Renderer renderer;
    renderer.Init(kWidth, kHeight, pixels.data());
    renderer.ClearLayers();
    renderer.BeginFrameStats();
    renderer.Clear();

    renderer.DrawElement(DRAW_TRIANGLES, 0, 3);

    DefaultShader shader;
    shader.model_matrix = glm::mat4(1.0f);
    shader.view_matrix = glm::mat4(1.0f);
    shader.project_matrix = glm::mat4(1.0f);
    renderer.UseProgram(&shader);

    renderer.DrawElement(DRAW_TRIANGLES, 0, 3);

    const uint32_t vao = renderer.GenVertexArray();
    renderer.BindVertexArray(vao);
    renderer.DrawElement(DRAW_TRIANGLES, 0, 3);
    renderer.DrawElement(DRAW_TRIANGLES, 0, 0);

    const uint32_t short_ebo = renderer.GenBuffer();
    const uint32_t short_indices[] = {0};
    renderer.BindBuffer(ELEMENT_ARRAY_BUFFER, short_ebo);
    renderer.BufferData(ELEMENT_ARRAY_BUFFER, sizeof(short_indices), const_cast<uint32_t*>(short_indices));
    renderer.DrawElement(DRAW_TRIANGLES, 0, 3);

    const uint32_t ebo = renderer.GenBuffer();
    const uint32_t indices[] = {0, 1, 2};
    renderer.BindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
    renderer.BufferData(ELEMENT_ARRAY_BUFFER, sizeof(indices), const_cast<uint32_t*>(indices));

    const uint32_t short_vbo = renderer.GenBuffer();
    const ValidationVertex vertices[] = {
        {-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
    };
    renderer.BindBuffer(ARRAY_BUFFER, short_vbo);
    renderer.BufferData(ARRAY_BUFFER, sizeof(vertices), const_cast<ValidationVertex*>(vertices));
    renderer.VertexAttributePointer(0, 4, sizeof(ValidationVertex), offsetof(ValidationVertex, px));
    renderer.VertexAttributePointer(1, 4, sizeof(ValidationVertex), offsetof(ValidationVertex, r));
    renderer.VertexAttributePointer(2, 2, sizeof(ValidationVertex), offsetof(ValidationVertex, u));
    renderer.DrawElement(DRAW_TRIANGLES, 0, 3);

    const auto& stats = renderer.GetFrameStats();
    std::cout << "draw command validation draw_calls=" << stats.draw_calls
              << " input_triangles=" << stats.input_triangles
              << " rasterized_fragments=" << stats.rasterized_fragments
              << " shaded_fragments=" << stats.shaded_fragments
              << std::endl;

    if(stats.draw_calls != 0 ||
       stats.input_triangles != 0 ||
       stats.rasterized_fragments != 0 ||
       stats.shaded_fragments != 0)
    {
        return 1;
    }

    std::cout << "draw command validation Pass" << std::endl;
    return 0;
}
