
#include "renderer.h"

#include <iostream>
#include <vector>

#include "pipeline_data.h"
#include "frame_buffer.h"
#include "raster_tool.h"
#include "buffer_object.h"
#include "vertex_array_object.h"
#include "clip_tool.h"
#include "texture.h"
#include "frame_buffer.h"
#include "perspective_camera.h"
#include "layer_stack.h"
#include "runtime/scoped_timer.h"

#include "test_layer.h"

Renderer::~Renderer()
{
    if(nullptr != layer_stack_)
    {
        delete layer_stack_;
        layer_stack_ = nullptr;
    }

    if(nullptr != current_frame_buffer_)
    {
        delete current_frame_buffer_;
        current_frame_buffer_ = nullptr;
    }

    for (auto iter : buffer_map_) 
    {
		delete iter.second;
	}
	buffer_map_.clear();

	for (auto iter : vao_map_) {
		delete iter.second;
	}
	vao_map_.clear();

    for (auto iter : texture_map_) {
		delete iter.second;
	}
	texture_map_.clear();

    if(nullptr != camera_)
	{
		delete camera_;
		camera_ = nullptr;
	}

}

void Renderer::Init(int32_t frame_width, int32_t frame_height, void* buffer)
{
    InitFrameBuffer(frame_width, frame_height, buffer);

    UpdateScreenMatrix(frame_width, frame_height);

    InitCamera();

    InitLayer();
}

void Renderer::BeginFrameStats()
{
    frame_stats_.Reset();
}

void Renderer::SetFrameTiming(double frame_ms, double update_ms, double render_ms, double present_ms)
{
    frame_stats_.frame_ms = frame_ms;
    frame_stats_.update_ms = update_ms;
    frame_stats_.render_ms = render_ms;
    frame_stats_.present_ms = present_ms;
}

const FrameStats& Renderer::GetFrameStats() const
{
    return frame_stats_;
}

void Renderer::Clear()
{
    if(nullptr != current_frame_buffer_)
    {
        current_frame_buffer_->FillColor();
        current_frame_buffer_->FillDepth();
    }
}

void Renderer::Render()
{
    if(nullptr != layer_stack_)
    {
        layer_stack_->Render();
    }
    
}

void Renderer::OnEvent(Event& ev)
{
    //处理事件
    if(ev.Name() == "KeyDownEvent")
    {
        KeyDownEvent& e = dynamic_cast<KeyDownEvent&>(ev);

        if(e.keycode == VK_ESCAPE) //ESC退出
        {
            if(exit_requested_callback_)
            {
                exit_requested_callback_();
            }
            return;
        }

        if(camera_)
        {
            camera_->OnKeyDown(e.keycode);
        }
        return;
    }

    if(ev.Name() == "KeyUpEvent")
    {
        KeyUpEvent& e = dynamic_cast<KeyUpEvent&>(ev);
        if(camera_)
        {
            camera_->OnKeyUp(e.keycode);
        }
        return;
    }

    if(ev.Name() == "MouseDownEvent")
    {
        MouseDownEvent& e = dynamic_cast<MouseDownEvent&>(ev);

        if(e.button == 2)
        {
            if(camera_)
            {
                camera_->OnRightMouseDown(e.posx, e.posy);
            }
        }  
        return;
    }

    if(ev.Name() == "MouseUpEvent")
    {
        MouseUpEvent& e = dynamic_cast<MouseUpEvent&>(ev);
        if(e.button == 2)
        {
            if(camera_)
            {
                camera_->OnRightMouseUp(e.posx, e.posy);
            }
        } 
        return;
    }

    if(ev.Name() == "MouseMoveEvent")
    {
        MouseMoveEvent& e = dynamic_cast<MouseMoveEvent&>(ev);
        if(camera_)
        {
            camera_->OnMouseMove(e.posx, e.posy);
        }
        return;
    }
}

void Renderer::OnUpdate()
{
    if(nullptr != camera_)
    {
        camera_->Update(); //根据当前按键和鼠标，更新摄像机位置,方向,并重新计算View矩阵
    }

    if(nullptr != layer_stack_)
    {
        layer_stack_->Update();
    }    
}

void Renderer::SetExitRequestedCallback(std::function<void()> callback)
{
    exit_requested_callback_ = callback;
}

void Renderer::AddLayer(Layer* layer)
{
    if(layer_stack_ == nullptr)
    {
        layer_stack_ = new LayerStack();
    }

    layer_stack_->PushLayer(layer);
}

void Renderer::ClearLayers()
{
    if(layer_stack_ != nullptr)
    {
        layer_stack_->Clear();
    }
}

void Renderer::SetCamera(AbstractCamera* camera)
{
	if(nullptr != camera && nullptr != camera_)
	{
		delete camera_;
	}
	camera_ = camera;
}

AbstractCamera* Renderer::GetCamera() const
{
	return camera_;
}

uint32_t Renderer::GenBuffer()
{
    buffer_num_++;

    buffer_map_[buffer_num_] = new BufferObject();

    return buffer_num_;
}

void Renderer::DeleteBuffer(uint32_t vbo)
{
    auto it = buffer_map_.find(vbo);
    if(it == buffer_map_.end())
    {
        return;
    }

    delete it->second;

    buffer_map_.erase(vbo);
}

void Renderer::BindBuffer(Buffer_Type buffer_type, uint32_t buffer_id)
{
    if (buffer_type == ARRAY_BUFFER) 
    {
		current_vbo_ = buffer_id;
	}
	else if (buffer_type == ELEMENT_ARRAY_BUFFER) 
    {
		current_ebo_ = buffer_id;
	}
}

void Renderer::BufferData(Buffer_Type buffer_type, size_t data_size, void *data)
{
    uint32_t buffer_id = 0;
	if (buffer_type == ARRAY_BUFFER) 
    {
		buffer_id = current_vbo_;
	}
	else if (buffer_type == ELEMENT_ARRAY_BUFFER) 
    {
		buffer_id = current_ebo_;
	}
	else {
		assert(false);
	}

	auto iter = buffer_map_.find(buffer_id);
	if (iter == buffer_map_.end()) 
    {
		assert(false);
	}

	BufferObject* bufferObject = iter->second;
	bufferObject->SetBuffer(data, data_size);
}

uint32_t Renderer::GenVertexArray()
{
    vao_num_++;

    vao_map_[vao_num_] = new VertexArrayObject();

    return vao_num_;
}

void Renderer::DeleteVertexArray(uint32_t vao)
{
    auto it = vao_map_.find(vao);
    if(it == vao_map_.end())
    {
        return;
    }

    delete it->second;

    vao_map_.erase(vao);
}

void Renderer::BindVertexArray(uint32_t vao_id)
{
    current_vao_ = vao_id;
}

void Renderer::VertexAttributePointer(uint32_t vertex_attribute_id, uint32_t item_size, uint32_t stride, uint32_t offset)
{
    auto iter = vao_map_.find(current_vao_);
	if (iter == vao_map_.end()) 
    {
		assert(false);
	}

	auto vao = iter->second;
	vao->BindVertexAttribute(vertex_attribute_id, current_vbo_, item_size, stride, offset);
}

void Renderer::UseProgram(Shader* shader)
{
    current_shader_ = shader;
}

void Renderer::DrawElement(DRAW_MODE drawMode, uint32_t first, uint32_t count)
{
    DrawCommand command;
    if(!BuildDrawCommand(drawMode, first, count, command))
    {
        return;
    }

    RecordDrawCallStats(command);
    pipeline_scratch_.ResetForDraw(command.count);

    auto& vs_outputs = pipeline_scratch_.vertex_outputs;
    auto& clip_outputs = pipeline_scratch_.clip_outputs;
    auto& cull_outputs = pipeline_scratch_.cull_outputs;
    auto& raster_outputs = pipeline_scratch_.raster_outputs;

    if(!RunVertexStage(vs_outputs, command))
    {
        return;
    }

    if(!RunClipStage(command.draw_mode, vs_outputs, clip_outputs))
    {
        return;
    }

    RunPerspectiveDivideStage(clip_outputs);

    std::vector<VsOutput>* post_cull_outputs = RunCullStage(command.draw_mode, clip_outputs, cull_outputs);
    RunViewportStage(*post_cull_outputs);

    if(!RunRasterStage(command.draw_mode, *post_cull_outputs, raster_outputs))
    {
        return;
    }

    RunFragmentOutputStage(raster_outputs);
}

bool Renderer::BuildDrawCommand(DRAW_MODE drawMode, uint32_t first, uint32_t count, DrawCommand& command) const
{
    if(current_vao_ == 0 || nullptr == current_shader_ || count == 0)
    {
        return false;
    }

    if((drawMode == DRAW_TRIANGLES && count % 3 != 0) ||
       (drawMode == DRAW_LINES && count % 2 != 0))
    {
        std::cout << "Error: draw count does not match draw mode!" << std::endl;
        return false;
    }

    auto vao_iter = vao_map_.find(current_vao_);
    if(vao_iter == vao_map_.end())
    {
        std::cout << "Error: current vao is invalid!" << std::endl;
        return false;
    }

    auto ebo_iter = buffer_map_.find(current_ebo_);
    if(ebo_iter == buffer_map_.end())
    {
        std::cout << "Error: current ebo is invalid!" << std::endl;
        return false;
    }

    command.draw_mode = drawMode;
    command.first = first;
    command.count = count;
    command.vao = vao_iter->second;
    command.ebo = ebo_iter->second;
    return ValidateDrawCommandResources(command);
}

bool Renderer::ValidateDrawCommandResources(const DrawCommand& command) const
{
    if(nullptr == command.vao || nullptr == command.ebo || nullptr == command.ebo->GetBuffer())
    {
        std::cout << "Error: draw command references empty resources!" << std::endl;
        return false;
    }

    const size_t index_count = command.ebo->GetBufferSize() / sizeof(uint32_t);
    const size_t first = command.first;
    const size_t count = command.count;
    if(first > index_count || count > index_count - first)
    {
        std::cout << "Error: draw command index range exceeds EBO data!" << std::endl;
        return false;
    }

    const auto& binding_map = command.vao->GetVertexAttrDescMap();
    if(binding_map.empty())
    {
        std::cout << "Error: draw command has no vertex attributes!" << std::endl;
        return false;
    }

    const uint32_t* indices_data = reinterpret_cast<const uint32_t*>(command.ebo->GetBuffer());
    uint32_t max_index = 0;
    for(size_t i = first; i < first + count; ++i)
    {
        if(indices_data[i] > max_index)
        {
            max_index = indices_data[i];
        }
    }

    for(const auto& item : binding_map)
    {
        const auto& desc = item.second;
        if(desc.item_size == 0 || desc.stride == 0)
        {
            std::cout << "Error: vertex attribute has invalid layout!" << std::endl;
            return false;
        }

        auto vbo_iter = buffer_map_.find(desc.vbo_id);
        if(vbo_iter == buffer_map_.end() || nullptr == vbo_iter->second || nullptr == vbo_iter->second->GetBuffer())
        {
            std::cout << "Error: vertex attribute references invalid VBO!" << std::endl;
            return false;
        }

        const size_t required_bytes = static_cast<size_t>(desc.stride) * max_index +
            desc.offset + desc.item_size * sizeof(float);
        if(required_bytes > vbo_iter->second->GetBufferSize())
        {
            std::cout << "Error: vertex attribute range exceeds VBO data!" << std::endl;
            return false;
        }
    }

    return true;
}

void Renderer::RecordDrawCallStats(const DrawCommand& command)
{
    frame_stats_.draw_calls++;
    if(command.draw_mode == DRAW_TRIANGLES)
    {
        frame_stats_.input_triangles += command.count / 3;
    }
}

bool Renderer::RunVertexStage(std::vector<VsOutput>& vsOutputs, const DrawCommand& command)
{
    {
        ScopedTimer stage_timer(frame_stats_.vertex_stage_ms);
	    VertexShaderApply(vsOutputs, command.vao, command.ebo, command.first, command.count);
    }

    return !vsOutputs.empty();
}

bool Renderer::RunClipStage(DRAW_MODE drawMode, const std::vector<VsOutput>& vsOutputs,
    std::vector<VsOutput>& clipOutputs)
{
    {
        ScopedTimer stage_timer(frame_stats_.clip_stage_ms);
        ClipTool::Clip(drawMode, vsOutputs, clipOutputs, pipeline_scratch_.clip_work_a, pipeline_scratch_.clip_work_b);
    }

    if(clipOutputs.empty())
    {
        return false;
    }

    if(drawMode == DRAW_TRIANGLES)
    {
        frame_stats_.clipped_triangles += static_cast<uint32_t>(clipOutputs.size() / 3);
    }

    return true;
}

void Renderer::RunPerspectiveDivideStage(std::vector<VsOutput>& clipOutputs)
{
    {
        ScopedTimer stage_timer(frame_stats_.ndc_stage_ms);
        for(auto& output : clipOutputs)
        {
		    PerspectiveDivision(output);
	    }
    }
}

std::vector<VsOutput>* Renderer::RunCullStage(DRAW_MODE drawMode, std::vector<VsOutput>& clipOutputs,
    std::vector<VsOutput>& cullOutputs)
{
    std::vector<VsOutput>* post_cull_outputs = &clipOutputs;
    {
        ScopedTimer stage_timer(frame_stats_.cull_stage_ms);
        if(drawMode == DRAW_TRIANGLES && enable_cull_face_)
        {
            for(size_t i = 0; i + 2 < clipOutputs.size(); i += 3)
            {
			    if(!ClipTool::CullFace(front_face_link_style_, cull_which_face_, clipOutputs[i], clipOutputs[i + 1], clipOutputs[i + 2]))
                {
				    auto start = clipOutputs.begin() + i;
				    auto end = clipOutputs.begin() + i + 3;
				    cullOutputs.insert(cullOutputs.end(), start, end);
			    }
		    }
            post_cull_outputs = &cullOutputs;
        }
    }

    return post_cull_outputs;
}

void Renderer::RunViewportStage(std::vector<VsOutput>& postCullOutputs)
{
    {
        ScopedTimer stage_timer(frame_stats_.viewport_stage_ms);
        for(auto& output : postCullOutputs)
        {
		    ScreenMapping(output);
	    }
    }
}

bool Renderer::RunRasterStage(DRAW_MODE drawMode, const std::vector<VsOutput>& postCullOutputs,
    std::vector<VsOutput>& rasterOutputs)
{
    {
        ScopedTimer stage_timer(frame_stats_.raster_stage_ms);
        RasterTool::Rasterize(drawMode, postCullOutputs, rasterOutputs);
    }
    frame_stats_.rasterized_fragments += static_cast<uint32_t>(rasterOutputs.size());

    return !rasterOutputs.empty();
}

void Renderer::RunFragmentOutputStage(std::vector<VsOutput>& rasterOutputs)
{
    {
        ScopedTimer stage_timer(frame_stats_.fragment_output_stage_ms);

        //9 透视恢复处理阶段
        //离散出来的像素插值结果，需要乘以w_0值恢复到正常态
        for(auto& output : rasterOutputs)
        {
		    PerspectiveRecover(output);
	    }

        //10 FragmentShader
        //颜色输出处理
        FsOutput fs_output;
	    for(uint32_t i = 0; i < rasterOutputs.size(); ++i)
        {
		    current_shader_->FragmentShader(rasterOutputs[i], fs_output, texture_map_);
            frame_stats_.shaded_fragments++;

            //深度测试
		    if (enable_depth_test_ && !DepthTest(fs_output)) 
            {
                frame_stats_.depth_rejected_fragments++;
			    continue;
		    }

            //混合判断
            Color color = fs_output.color;
            if(enable_blend_)
            {
                color = BlendColor(fs_output.pixelPos.x, fs_output.pixelPos.y, color);
            }

		    current_frame_buffer_->SetPixelColor(fs_output.pixelPos.x, fs_output.pixelPos.y, color);
	    }
    }
}

void Renderer::PrintVao(uint32_t vao) const
{
    auto it = vao_map_.find(vao);
    if(it != vao_map_.end())
    {
        it->second->Print();
    }
}

Color Renderer::BlendColor(int32_t x, int32_t y, const Color &src_color)
{
    if(nullptr == current_frame_buffer_)
    {
        return src_color;
    }

    Color* dst_color = current_frame_buffer_->GetPixelColor(x, y);
    if(nullptr == dst_color)
    {
        return src_color;
    }

    float src_weight = src_color.a / 255.0f;

    Color result;
    result.r = src_color.r * src_weight + dst_color->r * (1.0f - src_weight);
    result.g = src_color.g * src_weight + dst_color->g * (1.0f - src_weight);
    result.b = src_color.b * src_weight + dst_color->b * (1.0f - src_weight);

    return result;
}

void Renderer::VertexShaderApply(
		std::vector<VsOutput>& vsOutputs,
		const VertexArrayObject* vao,
		const BufferObject* ebo,
		uint32_t first,
		uint32_t count)
{
    const auto& binding_map = vao->GetVertexAttrDescMap();
	const uint32_t* indices_data = reinterpret_cast<const uint32_t*>(ebo->GetBuffer());

    vsOutputs.clear();
    vsOutputs.reserve(count);

	for(uint32_t i = first; i < first + count; ++i)
    {
		const uint32_t index = indices_data[i];
		vsOutputs.emplace_back(current_shader_->VertexShader(binding_map, buffer_map_, index));
	}
}

void Renderer::PerspectiveDivision(VsOutput& vs_output)
{
    float coff = 1.0f / vs_output.position.w;

    vs_output.one_devide_w = coff;
	vs_output.position *= coff;
	vs_output.position.w = 1.0f;

    vs_output.color *= coff;
    vs_output.uv *= coff;
    vs_output.normal *= coff;

    Trim(vs_output);
}

void Renderer::ScreenMapping(VsOutput& vs_output)
{
    vs_output.position = screen_matrix_ * vs_output.position;
}

bool Renderer::DepthTest(const FsOutput &output)
{
	float oldDepth = current_frame_buffer_->GetPixelDepth(output.pixelPos.x, output.pixelPos.y);
	switch (depth_test_func_)
	{
	case DEPTH_LESS:
		if (output.depth < oldDepth) 
        {
			current_frame_buffer_->SetOnePixelDepth(output.pixelPos.x, output.pixelPos.y, output.depth);
			return true;
		}
		else 
        {
			return false;
		}
		break;
	case DEPTH_GREATER:
		if (output.depth > oldDepth) 
        {
			current_frame_buffer_->SetOnePixelDepth(output.pixelPos.x, output.pixelPos.y, output.depth);
			return true;
		}
		else 
        {
			return false;
		}
		break;
	default:
		return false;
		break;
	}
}

void Renderer::PerspectiveRecover(VsOutput& vs_output)
{
    vs_output.color /= vs_output.one_devide_w;
    vs_output.uv /= vs_output.one_devide_w;
    vs_output.normal /= vs_output.one_devide_w;
}

void Renderer::Trim(VsOutput & vsOutput)
{
    //修剪毛刺,边界求交点的时候，可能会产生超过-1-1现象
	if (vsOutput.position.x < -1.0f) 
    {
		vsOutput.position.x = -1.0f;
	}

	if (vsOutput.position.x > 1.0f) 
    {
		vsOutput.position.x = 1.0f;
	}

	if (vsOutput.position.y < -1.0f) 
    {
		vsOutput.position.y = -1.0f;
	}

	if (vsOutput.position.y > 1.0f) 
    {
		vsOutput.position.y = 1.0f;
	}

	if (vsOutput.position.z < -1.0f) 
    {
		vsOutput.position.z = -1.0f;
	}

	if (vsOutput.position.z > 1.0f) 
    {
		vsOutput.position.z = 1.0f;
	}
}

void Renderer::InitCamera()
{
	if(nullptr == camera_)
	{
		camera_ = new PerspectiveCamera(glm::radians(60.0f), 
            (float)current_frame_buffer_->GetWidth() / (float)current_frame_buffer_->GetHeight(), 0.1f, 100.0f, { 0.0f, 1.0f, 0.0f });	
	}
}

void Renderer::UpdateScreenMatrix(int32_t screen_width, int32_t screen_height)
{
    screen_matrix_ = 
    {
        screen_width / 2, 0.0f, 0.0f, screen_width / 2,
        0.0f, screen_height / 2, 0.0f, screen_height / 2,
        0.0f, 0.0f, 0.5f, 0.5f, 
        0.0f, 0.0f, 0.0f, 1.0f
    };

    screen_matrix_ = glm::transpose(screen_matrix_); //glm是列优先存储, 需要转置
}

void Renderer::InitFrameBuffer(int32_t frame_width, int32_t frame_height, void* buffer)
{
    if(nullptr != current_frame_buffer_)
    {
        delete current_frame_buffer_;
    }

    current_frame_buffer_ = new FrameBuffer();
    current_frame_buffer_->Init(frame_width, frame_height, (Color*)buffer);
}

void Renderer::InitLayer()
{
    ClearLayers();
    AddLayer(new TestLayer(this));
}

void Renderer::Enable(ENABLE_TYPE param)
{
    switch (param)
    {
    case CULL_FACE:
        enable_cull_face_ = true; 
        break;
    case DEPTH_TEST:
        enable_depth_test_ = true;
        break;
    case COLOR_BLEND:
        enable_blend_ = true;
        break;
    default:
        break;
    }
}

void Renderer::Disable(ENABLE_TYPE param)
{
    switch (param)
    {
    case CULL_FACE:
        enable_cull_face_ = false; 
        break;
    case DEPTH_TEST:
        enable_depth_test_ = false;
        break;
    case COLOR_BLEND:
        enable_blend_ = false;
        break;
    default:
        break;
    }
}

void Renderer::SetFrontFaceType(FACE_FRONT_TYPE front_face_link_style)
{
    front_face_link_style_ = front_face_link_style;
}

void Renderer::SetCullFaceType(CULL_FACE_TYPE cull_which_face)
{
    cull_which_face_ = cull_which_face;
}

void Renderer::SetDepthTestFunc(DEPTH_TEST_FUNC depth_test_func)
{
    depth_test_func_ = depth_test_func;
}

uint32_t Renderer::GenTexture()
{
    texture_num_++;
	texture_map_.insert(std::make_pair(texture_num_, new Texture()));

	return texture_num_;
}

void Renderer::DeleteTexture(uint32_t tex_id)
{
    auto iter = texture_map_.find(tex_id);
	if (iter == texture_map_.end()) 
    {
		return;
	}

	delete iter->second;
	texture_map_.erase(iter);
}

void Renderer::BindTexture(uint32_t tex_id)
{
    current_texture_ = tex_id;
}

void Renderer::TexImage2D(uint32_t width, uint32_t height, void* data)
{
    if (!current_texture_) 
    {
		return;
	}

	auto iter = texture_map_.find(current_texture_);
	if (iter == texture_map_.end()) 
    {
		return;
	}

	auto texture = iter->second;
	texture->SetBufferData(width, height, data);
}

void Renderer::TexParameter(TEXTURE_PARAMETER_TYPE param, uint32_t value)
{
    if (!current_texture_) 
    {
		return;
	}

	auto iter = texture_map_.find(current_texture_);
	if (iter == texture_map_.end()) 
    {
		return;
	}
	auto texture = iter->second;
	texture->SetParameter(param, value);
}
