#ifndef _RENDER_TEXTURE_H
#define _RENDER_TEXTURE_H

#include "glm/glm.hpp"
#include "pipeline_data.h"

class Texture
{
public:
	Texture() = default;
	~Texture();
	Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

	//默认都是rgba格式
	void SetBufferData(uint32_t width, uint32_t height, void* data);

	glm::vec4 GetColor(float u, float v);

	void SetParameter(TEXTURE_PARAMETER_TYPE type, uint32_t value);

private:
	void CheckWrap(float& n, uint32_t type);

private:
	Color* buffer_ { nullptr };
	uint32_t width_ { 0 };
	uint32_t height_ { 0 };

	TEXTURE_FILTER_TYPE filter_ { TEXTURE_FILTER_NEAREST };
	TEXTURE_WRAP_TYPE wrapu_ { TEXTURE_WRAP_REPEAT };
	TEXTURE_WRAP_TYPE wrapv_ { TEXTURE_WRAP_REPEAT };
};

#endif