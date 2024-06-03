﻿#ifndef _RENDER_RENDERER_H
#define _RENDER_RENDERER_H_

#include <Windows.h>

#include <map>
#include <vector>

#include "frame_buffer.h"
#include "pipeline_data.h"
#include "image.h"
#include "buffer_object.h"
#include "vertex_array_object.h"
#include "shader.h"


class Renderer
{
public:
    Renderer() = default;
    ~Renderer();
    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;

    void InitFrameBuffer(LONG frame_width, LONG frame_height, void *buffer = nullptr);

    void Clear();

    inline void SetColorBlend(bool start_color_blend) { start_color_blend_ = start_color_blend; }

    inline void SetBilinearSample(bool start_bilinear_sample) { start_bilinear_sample_ = start_bilinear_sample; }

    inline void SetTextureUvWrap(TextureUvWrap texture_uv_wrap) { texture_uv_wrap_ = texture_uv_wrap; }

    void SetTexture(Image *texture) { texture_ = texture; }

    // =============================仿OpenGL接口================================
    uint32_t GenBuffer();
    void DeleteBuffer(uint32_t vbo);
    void BindBuffer(uint32_t buffer_type, uint32_t buffer_id);
	void BufferData(uint32_t buffer_type, size_t data_size, void* data);

    uint32_t GenVertexArray();
    void DeleteVertexArray(uint32_t vao);
    void BindVertexArray(uint32_t vao_id);
	void VertexAttributePointer(uint32_t binding, uint32_t item_size, uint32_t stride, uint32_t offset);

    void UseProgram(Shader* shader);
    void DrawElement(uint32_t drawMode, uint32_t first, uint32_t count);

    void PrintVao(uint32_t vao) const;

    
    void Enable(uint32_t param);

	void Disable(uint32_t param);

	//cull face
	void SetFrontFaceLinkStyle(uint32_t front_face_link_style);

	void SetCullWhichFace(uint32_t cull_which_face);
private:
    Color BlendColor(LONG x, LONG y, const Color &src_color);

    Color NearestUvSample(glm::vec2 &uv);
    Color BilinearUvSample(glm::vec2 &uv);

    void CheckUv(float &ux, float &uy);
    float Fracpart(float num);

    void VertexShaderApply(std::vector<VsOutput>& vsOutputs, const VertexArrayObject* vao,
		const BufferObject* ebo, uint32_t first, uint32_t count);

    void PerspectiveDivision(VsOutput& vs_output);

    void ScreenMapping(VsOutput& vs_output);
private:
    FrameBuffer *current_frame_buffer_ { nullptr };
    bool start_color_blend_ { false };
    bool start_bilinear_sample_ { false };
    TextureUvWrap texture_uv_wrap_ { WrapRepeat };
    Image *texture_ { nullptr };



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

    Shader* current_shader_{ nullptr };
	glm::mat4 screen_matrix_;

    //cull face
	bool enable_cull_face_ { true };
	uint32_t front_face_link_style_ { FRONT_FACE_CCW }; 
	uint32_t cull_which_face_ { BACK_FACE };
};

#endif