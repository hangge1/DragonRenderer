#ifndef _RENDER_SHADER_DEFAULT_SHADER_H_
#define _RENDER_SHADER_DEFAULT_SHADER_H_

#include "shader.h"

#include <map>

#include "glm/glm.hpp"
#include "pipeline_data.h"

class DefaultShader: public Shader 
{
public:
	DefaultShader() = default;
	~DefaultShader() = default;

	VsOutput vertexShader(
		const std::map<uint32_t, BindingDescription>& bindingMap,
		const std::map<uint32_t, BufferObject*>& bufferMap,
		const uint32_t& index
	) override;

	void fragmentShader(const VsOutput& input, FsOutput& output) override;

public:
	//uniforms
	glm::mat4 model_matrix;
	glm::mat4 view_matrix;
	glm::mat4 project_matrix;
};

#endif