#ifndef _RENDER_SHADER_LAMBERT_LIGHT_SHADER_H_
#define _RENDER_SHADER_LAMBERT_LIGHT_SHADER_H_

#include "shader.h"

#include <map>

#include "glm/glm.hpp"
#include "pipeline_data.h"
#include "buffer_object.h"
#include "texture.h"

struct DirectionalLight 
{
	glm::vec3 color;
	glm::vec3 direction;
};

struct EnvironmentLight
{
    glm::vec3 color;
};

class LambertLightShader : public Shader
{
public:
	LambertLightShader() = default;
	~LambertLightShader() = default;
    LambertLightShader(const LambertLightShader&) = delete;
    LambertLightShader& operator=(const LambertLightShader&) = delete;


	VsOutput VertexShader(
		const std::map<uint32_t, BindingDescription>& bindingMap,
		const std::map<uint32_t, BufferObject*>& bufferMap,
		const uint32_t& index
	) override;

	void FragmentShader(const VsOutput& input, FsOutput& output , const std::map<uint32_t, Texture*>& textures) override;

public:
	//uniforms
	glm::mat4 model_matrix;
	glm::mat4 view_matrix;
	glm::mat4 project_matrix;

	uint32_t diffuse_texture { 0 };

    DirectionalLight directional_light_;
    EnvironmentLight environment_light_;
};


#endif