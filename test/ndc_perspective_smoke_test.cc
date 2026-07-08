#include <cstdint>
#include <iostream>
#include <map>
#include <vector>

#include "pipeline_data.h"
#include "renderer.h"
#include "shader.h"

struct PerspectiveSmokeVertex
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

class ClipSpaceShader final : public Shader
{
public:
    VsOutput VertexShader(
        const std::map<uint32_t, VertexAttrDescription>& bindingMap,
        const std::map<uint32_t, BufferObject*>& bufferMap,
        const uint32_t& index) override
    {
        VsOutput output;
        output.position = GetVector(bindingMap, bufferMap, 0, index);
        output.color = GetVector(bindingMap, bufferMap, 1, index);
        output.uv = GetVector(bindingMap, bufferMap, 2, index);
        return output;
    }

    void FragmentShader(const VsOutput& input, FsOutput& output,
        const std::map<uint32_t, Texture*>& textures) override
    {
        (void)textures;
        output.pixelPos.x = static_cast<int>(input.position.x);
        output.pixelPos.y = static_cast<int>(input.position.y);
        output.depth = input.position.z;
        output.color = VectorToRGBA(input.color);
    }
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
    renderer.Disable(DEPTH_TEST);

    const PerspectiveSmokeVertex vertices[] = {
        {-0.50f, -0.50f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
        { 0.75f, -0.50f, 0.0f, 2.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f},
        { 0.00f,  0.25f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f, 1.0f},
    };
    const uint32_t indices[] = {0, 1, 2};

    const uint32_t vbo = renderer.GenBuffer();
    renderer.BindBuffer(ARRAY_BUFFER, vbo);
    renderer.BufferData(ARRAY_BUFFER, sizeof(vertices), const_cast<PerspectiveSmokeVertex*>(vertices));

    const uint32_t ebo = renderer.GenBuffer();
    renderer.BindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
    renderer.BufferData(ELEMENT_ARRAY_BUFFER, sizeof(indices), const_cast<uint32_t*>(indices));

    const uint32_t vao = renderer.GenVertexArray();
    renderer.BindVertexArray(vao);
    renderer.BindBuffer(ARRAY_BUFFER, vbo);
    renderer.VertexAttributePointer(0, 4, sizeof(PerspectiveSmokeVertex), offsetof(PerspectiveSmokeVertex, px));
    renderer.VertexAttributePointer(1, 4, sizeof(PerspectiveSmokeVertex), offsetof(PerspectiveSmokeVertex, r));
    renderer.VertexAttributePointer(2, 2, sizeof(PerspectiveSmokeVertex), offsetof(PerspectiveSmokeVertex, u));

    ClipSpaceShader shader;
    renderer.UseProgram(&shader);
    renderer.BindVertexArray(vao);
    renderer.BindBuffer(ELEMENT_ARRAY_BUFFER, ebo);
    renderer.DrawElement(DRAW_TRIANGLES, 0, 3);

    uint32_t colored_pixels = 0;
    uint32_t sum_r = 0;
    uint32_t sum_g = 0;
    uint32_t sum_b = 0;
    uint32_t sum_a = 0;
    for(const auto& pixel : pixels)
    {
        if(pixel.a != 0)
        {
            colored_pixels++;
            sum_r += pixel.r;
            sum_g += pixel.g;
            sum_b += pixel.b;
            sum_a += pixel.a;
        }
    }

    const uint64_t checksum = ComputeChecksum(pixels);
    const auto& stats = renderer.GetFrameStats();
    std::cout << "ndc perspective smoke colored_pixels=" << colored_pixels
              << " checksum=" << checksum
              << " sum_rgba=" << sum_r << "," << sum_g << "," << sum_b << "," << sum_a
              << " draw_calls=" << stats.draw_calls
              << " input_triangles=" << stats.input_triangles
              << " rasterized_fragments=" << stats.rasterized_fragments
              << " shaded_fragments=" << stats.shaded_fragments
              << " depth_rejected_fragments=" << stats.depth_rejected_fragments
              << std::endl;

    if(stats.draw_calls != 1 ||
       stats.input_triangles != 1 ||
       stats.rasterized_fragments != 21 ||
       stats.shaded_fragments != 21 ||
       stats.depth_rejected_fragments != 0 ||
       colored_pixels != 21 ||
       sum_r != 1684 ||
       sum_g != 995 ||
       sum_b != 2650 ||
       sum_a != 5355 ||
       checksum != 6166181789625516309ull)
    {
        return 1;
    }

    std::cout << "ndc perspective smoke Pass" << std::endl;
    return 0;
}
