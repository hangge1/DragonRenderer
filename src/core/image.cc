﻿
#include "image.h"

#include <string>

#include "pipeline_data.h"
#include "stb_image.h"

Image::Image(const uint32_t& width, const uint32_t& height, Color* data)
    : width_(width), height_(height)
{
    if (nullptr != data) 
    {
		data_ = new Color[width_ * height_];
		memcpy(data_, data, sizeof(Color) * width_ * height_);
	}
}

Image::~Image()
{
    if (nullptr != data_) 
    {
		delete[] data_;
        data_ = nullptr;
	}
}

Image* Image::createImage(const std::string& path)
{
    int picType { 0 };
	int width{ 0 }, height{ 0 };

	//stbimage读入的图片,原点在左上角，y轴是向下生长的
	//我方图形程序认为，图片应该是左下角为0，0；故需要翻转y轴
	stbi_set_flip_vertically_on_load(true);

	//由于我们是BGRA的格式，图片是RGBA的格式，所以得交换下R&B
	unsigned char* bits = stbi_load(path.c_str(), &width, &height, &picType, STBI_rgb_alpha);

	//数据层面直接操作，产生bgra序列的数据结构
	for (int i = 0; i < width * height * 4; i += 4)
	{
        std::swap(bits[i], bits[i+2]);
	}

	Image* image = new Image(width, height, (Color*)bits);

	stbi_image_free(bits);

	return image;
}

void Image::destroyImage(Image* image)
{
    if (nullptr != image) 
    {
		delete image;
        image = nullptr;
	}
}

Image* Image::CreateImageFromMemory(
	const std::string& path,
	unsigned char* dataIn,
	uint32_t widthIn,
	uint32_t heightIn
) 
{
	int picType = 0;
	int width{ 0 }, height{ 0 };

	//记录了整个数据的大小
	uint32_t dataInSize = 0;

	//一个fbx模型有可能打包进来jpg，带有压缩格式的图片情况下，height可能为0，width就代表了整个图片的大小
	if (!heightIn) 
	{
		dataInSize = widthIn;
	}
	else 
	{
		dataInSize = widthIn * heightIn;
	}

	//我们现在拿到的dataIn，并不是展开的位图数据，有可能是一个jpg png等格式的图片数据流
	unsigned char* bits = stbi_load_from_memory(dataIn, dataInSize, &width, &height, &picType, STBI_rgb_alpha);
	for (int i = 0; i < width * height * 4; i += 4)
	{
		unsigned char tmp = bits[i];
		bits[i] = bits[i + 2];
		bits[i + 2] = tmp;
	}

	Image* image = new Image(width, height, (Color*)bits);

	return image;
}