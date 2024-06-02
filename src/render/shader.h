#ifndef _RENDER_SHADER_H
#define _RENDER_SHADER_H

#include <map>

#include "buffer_object.h"
#include "pipeline_data.h"

class Shader
{
public:
	Shader() = default;
	~Shader() = default;
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

	virtual VsOutput vertexShader(
		//vao
		const std::map<uint32_t, BindingDescription>& bindingMap,

		//vbo
		const std::map<uint32_t,BufferObject*>& bufferMap,

		const uint32_t& index
	) = 0;

	virtual void fragmentShader(const VsOutput& input, FsOutput& output) = 0;
public:

	glm::vec4 getVector(
		const std::map<uint32_t, BindingDescription>& bindingMap,
		const std::map<uint32_t, BufferObject*>& bufferMap,
		const uint32_t& attributeLocation,
		const uint32_t& index);

	Color VectorToRGBA(const glm::vec4& v);
};

#endif