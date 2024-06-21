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

	void SetParameter(uint32_t type, uint32_t value);

private:
	void CheckWrap(float& n, uint32_t type);

private:
	Color* buffer_ { nullptr };
	uint32_t width_ { 0 };
	uint32_t height_ { 0 };

	uint32_t filter_ { TEXTURE_FILTER_NEAREST };
	uint32_t wrapu_ { TEXTURE_WRAP_REPEAT };
	uint32_t wrapv_ { TEXTURE_WRAP_REPEAT };
};

#endif