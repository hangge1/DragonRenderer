#ifndef _RENDER_RENDERER_H
#define _RENDER_RENDERER_H_

#include <Windows.h>

#include <map>

#include "frame_buffer.h"
#include "pixel.h"
#include "image.h"
#include "buffer_object.h"
#include "vertex_array_object.h"

enum TextureUvWrap
{
    WrapRepeat,
    WrapMirror
};

class Renderer
{
public:
    Renderer() = default;
    ~Renderer();
    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;

    void InitFrameBuffer(LONG frame_width, LONG frame_height, void *buffer = nullptr);

    void ClearFrameBuffer();

    void DrawPixel(LONG x_pox, LONG y_pox, Color &pixel_color);

    void DrawPixel(Pixel &point);

    void DrawLine(Pixel &start, Pixel &end);

    void DrawTriangle(Pixel &p1, Pixel &p2, Pixel &p3);

    void DrawPicture(const Image &image);

    void DrawPictureOnBlend(const Image &image, unsigned char src_alpha);

    inline void SetColorBlend(bool start_color_blend) { start_color_blend_ = start_color_blend; }

    inline void SetBilinearSample(bool start_bilinear_sample) { start_bilinear_sample_ = start_bilinear_sample; }

    inline void SetTextureUvWrap(TextureUvWrap texture_uv_wrap) { texture_uv_wrap_ = texture_uv_wrap; }

    void SetTexture(Image *texture) { texture_ = texture; }

    // 仿OpenGL接口
    uint32_t GenBuffer();
    void DeleteBuffer(uint32_t vbo);
    void BindBuffer(const uint32_t& buffer_type, const uint32_t& buffer_id);
	void BufferData(const uint32_t& buffer_type, size_t data_size, void* data);

    uint32_t GenVertexArray();
    void DeleteVertexArray(uint32_t vao);
    void BindVertexArray(const uint32_t& vao_id);
	void VertexAttributePointer(
		const uint32_t& binding,
		const uint32_t& item_size,
		const uint32_t& stride,
		const uint32_t& offset);

    void PrintVao(const uint32_t& vao) const;
private:
    Color BlendColor(LONG x, LONG y, const Color &src_color);

    Color NearestUvSample(glm::vec2 &uv);
    Color BilinearUvSample(glm::vec2 &uv);

    void CheckUv(float &ux, float &uy);
    float Fracpart(float num);

private:
    FrameBuffer *current_frame_buffer_{nullptr};
    bool start_color_blend_{false};
    bool start_bilinear_sample_{false};
    TextureUvWrap texture_uv_wrap_{WrapRepeat};
    Image *texture_{nullptr};

    // 仿OpenGL数据结构
    // VBO相关/EBO也存在内部
    uint32_t buffer_num_{0};
    std::map<uint32_t, BufferObject *> buffer_map_;

    // VAO相关
    uint32_t vao_num_{ 0 };
    std::map<uint32_t, VertexArrayObject *> vao_map_;

    //状态
    uint32_t current_vbo_{ 0 };
	uint32_t current_ebo_{ 0 };
	uint32_t current_vao_{ 0 };
};

#endif