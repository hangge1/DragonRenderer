
#include "renderer.h"

#include <iostream>
#include <vector>

#include "pipeline_data.h"
#include "frame_buffer.h"
#include "raster_tool.h"
#include "image.h"
#include "buffer_object.h"
#include "vertex_array_object.h"
#include "clip_tool.h"

Renderer::~Renderer()
{
    if(nullptr != current_frame_buffer_)
    {
        delete current_frame_buffer_;
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
}

void Renderer::InitFrameBuffer(LONG frame_width, LONG frame_height, void* buffer)
{
    if(nullptr != current_frame_buffer_)
    {
        delete current_frame_buffer_;
    }

    current_frame_buffer_ = new FrameBuffer();
    current_frame_buffer_->InitFrame(frame_width, frame_height, (Color*)buffer);


    int screen_width = frame_width;
    int screen_height = frame_height;
    screen_matrix_ = {
        screen_width / 2, 0.0f, 0.0f, screen_width / 2,
        0.0f, screen_height / 2, 0.0f, screen_height / 2,
        0.0f, 0.0f, 0.5f, 0.5f, 
        0.0f, 0.0f, 0.0f, 1.0f
    };

    screen_matrix_ = glm::transpose(screen_matrix_);
}

void Renderer::Clear()
{
    if(nullptr != current_frame_buffer_)
    {
        current_frame_buffer_->FillColor(Color(0, 0, 0, 0));
        current_frame_buffer_->FillDepth();
    }
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

void Renderer::BindBuffer(uint32_t buffer_type, uint32_t buffer_id)
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

void Renderer::BufferData(uint32_t buffer_type, size_t data_size, void *data)
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

void Renderer::VertexAttributePointer(uint32_t binding, uint32_t item_size, uint32_t stride, uint32_t offset)
{
    auto iter = vao_map_.find(current_vao_);
	if (iter == vao_map_.end()) 
    {
		assert(false);
	}

	auto vao = iter->second;
	vao->Bind(binding, current_vbo_, item_size, stride, offset);
}

void Renderer::UseProgram(Shader* shader)
{
    current_shader_ = shader;
}

void Renderer::DrawElement(uint32_t drawMode, uint32_t first, uint32_t count)
{
    if(current_vao_ == 0 || nullptr == current_shader_ || count == 0)
    {
        return;
    }

    //1 获取vao
    auto vao_iter = vao_map_.find(current_vao_);
    if(vao_iter == vao_map_.end())
    {
        std::cout << "Error: current vao is invalid!" << std::endl;
        return;
    }

    VertexArrayObject* vao = vao_iter->second;
    auto& binding_map = vao->GetBindingMap();

    //2 获取ebo
    auto ebo_iter = buffer_map_.find(current_ebo_);
    if(ebo_iter == buffer_map_.end())
    {
        std::cout << "Error: current ebo is invalid!" << std::endl;
        return;
    }

    const BufferObject* ebo = ebo_iter->second;

    //3 执行VertexShader
    //按照ebo的index顺序，处理顶点，依次通过顶点着色器
    std::vector<VsOutput> vs_outputs {};
	VertexShaderApply(vs_outputs, vao, ebo, first, count);

    if(vs_outputs.empty())
    {
        return;
    }

    //4 Clip剪裁
    std::vector<VsOutput> clip_outputs {};
    ClipTool::Clip(drawMode, vs_outputs, clip_outputs);

    if(clip_outputs.empty())
    {
        return;
    }


    //5 NDC处理：坐标转换为NDC
    for (auto& output : clip_outputs) 
    {
		PerspectiveDivision(output);
	}

    //6 背面剔除
    std::vector<VsOutput> cull_outputs {};
    if(drawMode == DRAW_TRIANGLES && enable_cull_face_)
    {
        for (int i = 0; i < clip_outputs.size() - 2; i += 3) {
			if (!ClipTool::CullFace(front_face_link_style_, cull_which_face_, clip_outputs[i], clip_outputs[i + 1], clip_outputs[i + 2])) {
				auto start = clip_outputs.begin() + i;
				auto end = clip_outputs.begin() + i + 3;
				cull_outputs.insert(cull_outputs.end(), start, end);
			}
		}
    }
    else
    {
        cull_outputs = clip_outputs;
    }



    //7 屏幕映射
    //转化坐标到屏幕空间
    for (auto& output : cull_outputs) 
    {
		ScreenMapping(output);
	}


    //8 光栅化
    //离散出所有Fragment
    std::vector<VsOutput> raster_outputs = RasterTool::Rasterize(drawMode, cull_outputs);

    if(raster_outputs.empty()) 
    {
        return;
    }

    //9 透视恢复处理阶段
    //离散出来的像素插值结果，需要乘以w_0值恢复到正常态
    for (auto& output : raster_outputs) 
    {
		PerspectiveRecover(output);
	}


    //10 FragmentShader
    //颜色输出处理
    FsOutput fs_output;
	uint32_t pixel_pos = 0;
	for (uint32_t i = 0; i < raster_outputs.size(); ++i) 
    {
		current_shader_->fragmentShader(raster_outputs[i], fs_output);

        //深度测试
		if (enable_depth_test_ && !DepthTest(fs_output)) 
        {
			continue;
		}

		current_frame_buffer_->SetOnePixelColor(fs_output.pixelPos.x, fs_output.pixelPos.y, fs_output.color);
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

Color Renderer::BlendColor(LONG x, LONG y, const Color &src_color)
{
    if(nullptr == current_frame_buffer_)
    {
        return src_color;
    }

    Color* dst_color = current_frame_buffer_->GetFrameColor(x, y);
    if(nullptr == dst_color)
    {
        return src_color;
    }


    if(start_color_blend_)
    {
        float src_weight = src_color.a / 255.0f;

        Color result;
        result.r = src_color.r * src_weight + dst_color->r * (1.0f - src_weight);
        result.g = src_color.g * src_weight + dst_color->g * (1.0f - src_weight);
        result.b = src_color.b * src_weight + dst_color->b * (1.0f - src_weight);

        return result;
    }

    return src_color;
}

Color Renderer::NearestUvSample(glm::vec2 &uv)
{
    if(nullptr == texture_ || nullptr == current_frame_buffer_)
    {
        return Color();
    }

    CheckUv(uv.x, uv.y);

    int x = uv.x * (texture_->get_width() - 1);
    int y = uv.y * (texture_->get_height() - 1);

    return texture_->get_data()[y * texture_->get_width() + x];
}

Color Renderer::BilinearUvSample(glm::vec2 &uv)
{
    if(nullptr == texture_ || nullptr == current_frame_buffer_)
    {
        return Color();
    }

    CheckUv(uv.x, uv.y);

    float x = uv.x * (texture_->get_width() - 1);
    float y = uv.y * (texture_->get_height() - 1);

    int x0 = std::floor(x);
    int x1 = std::ceil(x);
    int y0 = std::floor(y);
    int y1 = std::ceil(y);

    if(x0 == texture_->get_width() - 1)
    {
        x0--;
        x1 = x0 + 1;
    }

    if(y0 == texture_->get_height() - 1)
    {
        y0--;
        y1 = y0 + 1;
    }

    float weight = x - x0;
    Color x_y0 = texture_->get_data()[y0 * texture_->get_width() + x1] * weight + 
                 texture_->get_data()[y0 * texture_->get_width() + x0] * (1.0f - weight);

    Color x_y1 = texture_->get_data()[y1 * texture_->get_width() + x1] * weight + 
                 texture_->get_data()[y1 * texture_->get_width() + x0] * (1.0f - weight);

    weight = y - y0;

    return x_y1 * weight + x_y0 * (1 - weight);
}

void Renderer::CheckUv(float &ux, float &uy)
{
    if(ux < 0.0f || ux > 1.0f)
    {
        ux = Fracpart(Fracpart(ux) + 1.0f);
        if(texture_uv_wrap_ == WrapMirror)
        {
            ux = 1.0f - ux;
        }
    }

    if(uy < 0.0f || uy > 1.0f)
    {
        uy = Fracpart(Fracpart(uy) + 1.0f);
        if(texture_uv_wrap_ == WrapMirror)
        {
            uy = 1.0f - uy;
        }
    }
}

float Renderer::Fracpart(float num)
{
    return num - (int)num;
}

 void Renderer::VertexShaderApply(
		std::vector<VsOutput>& vsOutputs,
		const VertexArrayObject* vao,
		const BufferObject* ebo,
		uint32_t first,
		uint32_t count)
{
    auto binding_map = vao->GetBindingMap();
	uint8_t* indicesData = ebo->GetBuffer();

	uint32_t index = 0;
	for (uint32_t i = first; i < first + count; ++i) {
		//获取Ebo中第i个index
		size_t indicesOffset = i * sizeof(uint32_t);
		memcpy(&index, indicesData + indicesOffset, sizeof(uint32_t));

		VsOutput output = current_shader_->vertexShader(binding_map, buffer_map_, index);
		vsOutputs.push_back(output);
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
}

void Renderer::ScreenMapping(VsOutput& vs_output)
{
    vs_output.position = screen_matrix_ * vs_output.position;
}

bool Renderer::DepthTest(const FsOutput &output)
{
	float oldDepth = current_frame_buffer_->GetFrameDepth(output.pixelPos.x, output.pixelPos.y);
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
}

void Renderer::Enable(uint32_t param)
{
    switch (param)
    {
    case CULL_FACE:
        enable_cull_face_ = true; 
        break;
    case DEPTH_TEST:
        enable_depth_test_ = true;
        break;
    default:
        break;
    }
}

void Renderer::Disable(uint32_t param)
{
    switch (param)
    {
    case CULL_FACE:
        enable_cull_face_ = false; 
        break;
    case DEPTH_TEST:
        enable_depth_test_ = false;
        break;
    default:
        break;
    }
}

void Renderer::SetFrontFaceLinkStyle(uint32_t front_face_link_style)
{
    front_face_link_style_ = front_face_link_style;
}

void Renderer::SetCullWhichFace(uint32_t cull_which_face)
{
    cull_which_face_ = cull_which_face;
}

void Renderer::SetDepthTestFunc(uint32_t depth_test_func)
{
    depth_test_func_ = depth_test_func;
}
