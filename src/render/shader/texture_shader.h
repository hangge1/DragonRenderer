#ifndef _RENDER_SHADER_TEXTURE_SHADER_H_
#define _RENDER_SHADER_TEXTURE_SHADER_H_

#include "shader.h"

#include <map>

#include "glm/glm.hpp"
#include "pipeline_data.h"
#include "buffer_object.h"
#include "texture.h"

class TextureShader : public Shader
{
public:
	TextureShader() = default;
	~TextureShader() = default;

	VsOutput vertexShader(
		const std::map<uint32_t, BindingDescription>& bindingMap,
		const std::map<uint32_t, BufferObject*>& bufferMap,
		const uint32_t& index
	) override;

	void fragmentShader(const VsOutput& input, FsOutput& output ,const std::map<uint32_t, Texture*>& textures) override;

public:
	//uniforms
	glm::mat4 model_matrix;
	glm::mat4 view_matrix;
	glm::mat4 project_matrix;

	uint32_t diffuse_texture { 0 };
};


#endif