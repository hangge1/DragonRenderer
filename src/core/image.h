#ifndef _CORE_IMAGE_H_
#define _CORE_IMAGE_H_

#include <iostream>

#include "pipeline_data.h"

class Image
{
public:
	Image(const uint32_t& width = 0, const uint32_t& height = 0, Color* data = nullptr);
	~Image();

	static Image* createImage(const std::string& path);
	static void destroyImage(Image* image);
	
    inline Color* get_data() const { return data_; }
    inline uint32_t get_width() const { return width_; }
    inline uint32_t get_height() const { return height_; }
private:
	uint32_t width_;
	uint32_t height_;
	Color* data_;
};

#endif