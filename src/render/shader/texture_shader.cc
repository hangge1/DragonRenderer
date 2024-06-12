
#include "texture_shader.h"

#include <iostream>
#include <map>

#include "texture.h"





VsOutput TextureShader::vertexShader(
	const std::map<uint32_t, BindingDescription>& binding_map,
	const std::map<uint32_t, BufferObject*>& buffer_map,
	const uint32_t& index) 
{
	VsOutput output;

	//取出Attribute数值
	glm::vec4 position = getVector(binding_map, buffer_map, 0, index);

	//变化为齐次坐标 
	position.w = 1.0f;
	glm::vec4 color = getVector(binding_map, buffer_map, 1, index);

	glm::vec2 uv = getVector(binding_map, buffer_map, 2, index);

	output.position = project_matrix * view_matrix * model_matrix * position;
	output.color = color;
	output.uv = uv;

    return output;
}

void TextureShader::fragmentShader(const VsOutput& input, FsOutput& output, const std::map<uint32_t, Texture*>& textures) 
{
	output.pixelPos.x = static_cast<int>(input.position.x);
	output.pixelPos.y = static_cast<int>(input.position.y);
	output.depth = input.position.z;

	//取出texture
	auto iter = textures.find(diffuse_texture);
	auto texture = iter->second;

	//计算颜色
	glm::vec4 diffuseColor = texture->GetColor(input.uv.x, input.uv.y);
	output.color = VectorToRGBA(diffuseColor);
}