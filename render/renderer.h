#ifndef _RENDER_RENDERER_H
#define _RENDER_RENDERER_H_

#include <Windows.h>

#include <functional>
#include <map>
#include <vector>

#include "buffer_object.h"
#include "draw_command.h"
#include "event.h"

#include "shader.h"
#include "texture.h"
#include "vertex_array_object.h"
#include "pipeline_scratch.h"
#include "runtime/frame_stats.h"

class FrameBuffer;
class AbstractCamera;
class InputState;
class Layer;
class LayerStack;

class Renderer
{
public:
    Renderer() = default;
    ~Renderer();
    Renderer(const Renderer &) = delete;
    Renderer(Renderer &&) = delete;
    Renderer &operator=(const Renderer &) = delete;
    Renderer &operator=(Renderer &&) = delete;

    void Init(int32_t frame_width, int32_t frame_height, void *buffer = nullptr);
    void ResizeRenderTarget(int32_t frame_width, int32_t frame_height, void *buffer = nullptr);
    void BeginFrameStats();
    void SetFrameTiming(double frame_ms, double update_ms, double render_ms, double present_ms);
    const FrameStats& GetFrameStats() const;

    void Clear();
    void Render();

    void OnEvent(Event& ev);
    void OnInput(const InputState& input);
    void OnUpdate();

    void SetExitRequestedCallback(std::function<void()> callback);
    void AddLayer(Layer* layer);
    void ClearLayers();

    void SetCamera(AbstractCamera* camera);
    AbstractCamera* GetCamera() const;
    

    // =============================仿OpenGL接口================================
    // VBO
    uint32_t GenBuffer();
    void DeleteBuffer(uint32_t vbo);
    void BindBuffer(Buffer_Type buffer_type, uint32_t buffer_id);
	void BufferData(Buffer_Type buffer_type, size_t data_size, void* data);

    // VAO
    uint32_t GenVertexArray();
    void DeleteVertexArray(uint32_t vao);
    void BindVertexArray(uint32_t vao_id);
	void VertexAttributePointer(uint32_t vertex_attribute_id, uint32_t item_size, uint32_t stride, uint32_t offset);
    //测试使用
    void PrintVao(uint32_t vao) const;

    // 使用shader
    void UseProgram(Shader* shader);

    // Draw Call
    void DrawElement(DRAW_MODE drawMode, uint32_t first, uint32_t count);

    // 深度测试、面片剔除、颜色混合
    void Enable(ENABLE_TYPE param);
	void Disable(ENABLE_TYPE param);

	//面片剔除（Cull）参数设置
	void SetFrontFaceType(FACE_FRONT_TYPE front_face_type);
	void SetCullFaceType(CULL_FACE_TYPE cull_which_face);

    //深度测试参数设置
    void SetDepthTestFunc(DEPTH_TEST_FUNC depth_test_func);

    //纹理
	uint32_t GenTexture();
	void DeleteTexture(uint32_t tex_id);
	void BindTexture(uint32_t tex_id);
	void TexImage2D(uint32_t width, uint32_t height, void* data);
	void TexParameter(TEXTURE_PARAMETER_TYPE param, uint32_t value);

private:
    bool BuildDrawCommand(DRAW_MODE drawMode, uint32_t first, uint32_t count, DrawCommand& command) const;
    bool ValidateDrawCommandResources(const DrawCommand& command) const;
    void RecordDrawCallStats(const DrawCommand& command);
    bool RunVertexStage(std::vector<VsOutput>& vsOutputs, const DrawCommand& command);
    bool RunClipStage(DRAW_MODE drawMode, const std::vector<VsOutput>& vsOutputs,
        std::vector<VsOutput>& clipOutputs);
    void RunPerspectiveDivideStage(std::vector<VsOutput>& clipOutputs);
    std::vector<VsOutput>* RunCullStage(DRAW_MODE drawMode, std::vector<VsOutput>& clipOutputs,
        std::vector<VsOutput>& cullOutputs);
    void RunViewportStage(std::vector<VsOutput>& postCullOutputs);
    bool RunRasterOutputStage(DRAW_MODE drawMode, const std::vector<VsOutput>& postCullOutputs,
        std::vector<VsOutput>& rasterOutputs);
    void ProcessFragmentOutputs(std::vector<VsOutput>& rasterOutputs);

    void VertexShaderApply(std::vector<VsOutput>& vsOutputs, const VertexArrayObject* vao,
		const BufferObject* ebo, uint32_t first, uint32_t count);

    Color BlendColor(int32_t x, int32_t y, const Color &src_color);

    void PerspectiveDivision(VsOutput& vs_output);

    void ScreenMapping(VsOutput& vs_output);

    bool DepthTest(const FsOutput& output);

    void PerspectiveRecover(VsOutput& vs_output);

    void Trim(VsOutput& vsOutput);

private:
    void InitCamera();

    void UpdateScreenMatrix(int32_t screen_width, int32_t screen_height);

    void InitFrameBuffer(int32_t frame_width, int32_t frame_height, void* buffer);

private:
    FrameBuffer* current_frame_buffer_ = nullptr; //帧缓冲
    AbstractCamera* camera_  = nullptr;
    LayerStack* layer_stack_ = nullptr;
    std::function<void()> exit_requested_callback_;

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
	FACE_FRONT_TYPE front_face_link_style_ { FRONT_FACE_CCW }; 
	CULL_FACE_TYPE cull_which_face_ { BACK_FACE };

    //深度测试相关
    bool enable_depth_test_ { true };
	DEPTH_TEST_FUNC depth_test_func_ { DEPTH_LESS };

    //颜色混合相关
    bool enable_blend_ { false };

    PipelineScratch pipeline_scratch_;
    FrameStats frame_stats_;
};

#endif
