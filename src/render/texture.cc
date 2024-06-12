

#include "texture.h"

#include <cstring>

#include "pipeline_data.h"
#include "glm/glm.hpp"

const float FRACTION(float v)
{
	return v - (int)v;
};

Texture::~Texture()
{
	if (buffer_ != nullptr)
	{
		delete[] buffer_;
		buffer_ = nullptr;
	}
}

void Texture::SetBufferData(uint32_t width, uint32_t height, void *data)
{
	if (buffer_ != nullptr)
	{
		delete[] buffer_;
		buffer_ = nullptr;
	}

	width_ = width;
	height_ = height;

	if (buffer_ == nullptr)
	{
		buffer_ = new Color[width * height];
	}

	std::memcpy(buffer_, data, width * height * sizeof(Color));
}

glm::vec4 Texture::GetColor(float u, float v)
{
	Color resultColor;

	// 处理uv坐标
	CheckWrap(u, wrapu_);
	CheckWrap(v, wrapv_);

	if (filter_ == TEXTURE_FILTER_NEAREST)
	{
		int x = std::round(u * (width_ - 1));
		int y = std::round(v * (height_ - 1));

		int position = y * width_ + x;
		resultColor = buffer_[position];
	}
	else if (filter_ == TEXTURE_FILTER_LINEAR)
	{
		float x = u * static_cast<float>(width_ - 1);
		float y = v * static_cast<float>(height_ - 1);

		int left = std::floor(x);
		int right = std::ceil(x);
		int bottom = std::floor(y);
		int top = std::ceil(y);

		// 对上下插值，得到左右
		float yScale = 0.0f;
		if (top == bottom)
		{
			yScale = 1.0f;
		}
		else
		{
			yScale = (y - static_cast<float>(bottom)) / static_cast<float>(top - bottom);
		}

		int positionLeftTop = top * width_ + left;
		int positionLeftBottom = bottom * width_ + left;
		int positionRightTop = top * width_ + right;
		int positionRightBottom = bottom * width_ + right;

		Color leftColor = buffer_[positionLeftBottom].Lerp(buffer_[positionLeftTop], yScale);
		Color rightColor = buffer_[positionRightBottom].Lerp(buffer_[positionRightTop], yScale);

		// 对左右插值，得到结果
		float xScale = 0.0f;
		if (right == left)
		{
			xScale = 1.0f;
		}
		else
		{
			xScale = (x - static_cast<float>(left)) / static_cast<float>(right - left);
		}

		resultColor = leftColor.Lerp(rightColor, xScale);
	}

	glm::vec4 result;
	result.x = static_cast<float>(resultColor.r) / 255.0;
	result.y = static_cast<float>(resultColor.g) / 255.0;
	result.z = static_cast<float>(resultColor.b) / 255.0;
	result.w = static_cast<float>(resultColor.a) / 255.0;

	return result;
}

void Texture::CheckWrap(float &n, uint32_t type)
{
	if (n > 1.0f || n < 0.0f)
	{
		n = FRACTION(n);

		switch (type)
		{
		case TEXTURE_WRAP_REPEAT:
			n = FRACTION(n + 1);
			break;
		case TEXTURE_WRAP_MIRROR:
			n = 1.0f - FRACTION(n + 1);
			break;
		default:
			break;
		}
	}
}

void Texture::SetParameter(uint32_t type, uint32_t value)
{
	switch (type)
	{
	case TEXTURE_FILTER:
		filter_ = value;
		break;
	case TEXTURE_WRAP_U:
		wrapu_ = value;
		break;
	case TEXTURE_WRAP_V:
		wrapv_ = value;
		break;
	default:
		break;
	}
}
