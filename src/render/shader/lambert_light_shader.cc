#include "lambert_light_shader.h"

#include <iostream>
#include <map>

#include "texture.h"

VsOutput LambertLightShader::VertexShader(
	const std::map<uint32_t, BindingDescription>& binding_map,
	const std::map<uint32_t, BufferObject*>& buffer_map,
	const uint32_t& index) 
{
	VsOutput output;

	//取出Attribute数值
	glm::vec4 position = GetVector(binding_map, buffer_map, 0, index);

	//变化为齐次坐标 
	position.w = 1.0f;
	//glm::vec4 color = GetVector(binding_map, buffer_map, 1, index);
    glm::vec3 normal = GetVector(binding_map, buffer_map, 1, index);
	glm::vec2 uv = GetVector(binding_map, buffer_map, 2, index);

	output.position = project_matrix * view_matrix * model_matrix * position;
	//output.color = color;
	output.normal = normal;
	output.uv = uv;

    return output;
}

void LambertLightShader::FragmentShader(const VsOutput& input, FsOutput& output, const std::map<uint32_t, Texture*>& textures) 
{
	output.pixelPos.x = static_cast<int>(input.position.x);
	output.pixelPos.y = static_cast<int>(input.position.y);
	output.depth = input.position.z;

	//取出法线
	auto normal = glm::normalize(input.normal);
	auto lightDirection = glm::normalize(directional_light_.direction);

	//取出texture
	auto iter = textures.find(diffuse_texture);
	auto texture = iter->second;

	//计算颜色
    glm::vec4 texColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    if (texture) 
    {
		texColor = texture->GetColor(input.uv.x, input.uv.y);
	}

    //计算漫反射
    float diff = glm::dot(normal, -lightDirection);
	diff = glm::clamp(diff, 0.0f, 1.0f); 
	glm::vec4 diffuseColor = texColor * diff * glm::vec4(directional_light_.color, 1.0f);

    //计算环境光
    glm::vec4  envColor = texColor * glm::vec4(environment_light_.color, 1.0f);

	output.color = VectorToRGBA(diffuseColor + envColor);
}