#include <cstdint>
#include <iostream>
#include <vector>

#include "glm/glm.hpp"

#include "pipeline_data.h"
#include "renderer.h"
#include "shader/default_shader.h"

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
