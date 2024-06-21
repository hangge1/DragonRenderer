

#include "default_shader.h"



#include "texture.h"

VsOutput DefaultShader::VertexShader(
	const std::map<uint32_t, BindingDescription>& binding_map,
	const std::map<uint32_t, BufferObject*>& buffer_map,
	const uint32_t& index
) 
{
	VsOutput output;

	//取出Attribute数值
	glm::vec4 position = GetVector(binding_map, buffer_map, 0, index);

	//变化为齐次坐标 
	position.w = 1.0f;
	glm::vec4 color = GetVector(binding_map, buffer_map, 1, index);

	glm::vec2 uv = GetVector(binding_map, buffer_map, 2, index);

	output.position = project_matrix * view_matrix * model_matrix * position;
	output.color = color;
	output.uv = uv;

	return output;
}

void DefaultShader::FragmentShader(const VsOutput& input, FsOutput& output, const std::map<uint32_t, Texture*>& textures) 
{
	output.pixelPos.x = static_cast<int>(input.position.x);
	output.pixelPos.y = static_cast<int>(input.position.y);
	output.depth = input.position.z;
	output.color = VectorToRGBA(input.color);
}