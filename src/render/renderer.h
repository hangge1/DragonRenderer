#ifndef _RENDER_RENDERER_H
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
#include "texture.h"


class Renderer
{
public:
    Renderer() = default;
    ~Renderer();
    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;

    void Init(int32_t frame_width, int32_t frame_height, void *buffer = nullptr);

    void Clear();

    // =============================仿OpenGL接口================================
    // VBO
    uint32_t GenBuffer();
    void DeleteBuffer(uint32_t vbo);
    void BindBuffer(uint32_t buffer_type, uint32_t buffer_id);
	void BufferData(uint32_t buffer_type, size_t data_size, void* data);

    // VAO
    uint32_t GenVertexArray();
    void DeleteVertexArray(uint32_t vao);
    void BindVertexArray(uint32_t vao_id);
	void VertexAttributePointer(uint32_t binding, uint32_t item_size, uint32_t stride, uint32_t offset);
    void PrintVao(uint32_t vao) const;

    // 使用shader
    void UseProgram(Shader* shader);

    // Draw Call
    void DrawElement(uint32_t drawMode, uint32_t first, uint32_t count);

    
    // 深度测试、面片剔除、颜色混合
    void Enable(uint32_t param);
	void Disable(uint32_t param);

	//面片剔除（Cull）参数设置
	void SetFrontFaceLinkStyle(uint32_t front_face_link_style);
	void SetCullWhichFace(uint32_t cull_which_face);

    //深度测试参数设置
    void SetDepthTestFunc(uint32_t depth_test_func);

    //纹理
	uint32_t GenTexture();
	void DeleteTexture(uint32_t tex_id);
	void BindTexture(uint32_t tex_id);
	void TexImage2D(uint32_t width, uint32_t height, void* data);
	void TexParameter(uint32_t param, uint32_t value);

private:
    void VertexShaderApply(std::vector<VsOutput>& vsOutputs, const VertexArrayObject* vao,
		const BufferObject* ebo, uint32_t first, uint32_t count);

    Color BlendColor(int32_t x, int32_t y, const Color &src_color);

    void PerspectiveDivision(VsOutput& vs_output);

    void ScreenMapping(VsOutput& vs_output);

    bool DepthTest(const FsOutput& output);

    void PerspectiveRecover(VsOutput& vs_output);

private:
    FrameBuffer* current_frame_buffer_ { nullptr };

    //=================仿OpenGL数据结构==================
    // VBO相关/EBO也存在内部
    uint32_t buffer_num_ { 0 };
    std::map<uint32_t, BufferObject*> buffer_map_;

    // VAO相关
    uint32_t vao_num_ { 0 };
    std::map<uint32_t, VertexArrayObject*> vao_map_;

    //纹理相关    
	uint32_t texture_num_ { 0 };
	std::map<uint32_t, Texture*> texture_map_;

    //状态
    uint32_t current_vbo_ { 0 };
	uint32_t current_ebo_ { 0 };
	uint32_t current_vao_ { 0 };
    uint32_t current_texture_ { 0 };
    Shader* current_shader_ { nullptr };

    //屏幕空间变换矩阵
	glm::mat4 screen_matrix_;

    //面片剔除相关
	bool enable_cull_face_ { false };
	uint32_t front_face_link_style_ { FRONT_FACE_CCW }; 
	uint32_t cull_which_face_ { BACK_FACE };

    //深度测试相关
	bool enable_depth_test_ { true };
	uint32_t depth_test_func_ { DEPTH_LESS };

    //颜色混合相关
    bool enable_blend_ { false };
};

#endif