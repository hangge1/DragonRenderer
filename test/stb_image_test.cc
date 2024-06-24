
#include <iostream>
#include <string>

#include "stb_image.h"


int Pass()
{
    std::cout << "thirdparts library stb_image Pass" << std::endl;
    return 0;
}

int FailPass()
{
     std::cout << "thirdparts library stb_image Can't Pass" << std::endl;
    return -1;
}


int main(int argc, char** argv)
{

    int width{ 0 }, height{ 0 };
    int picType { 0 };

    std::string path = "assets/texture/lufei.jpg";
	//由于我们是BGRA的格式，图片是RGBA的格式，所以得交换下R&B
	unsigned char* bits = stbi_load(path.c_str(), &width, &height, &picType, STBI_rgb_alpha);

    if(nullptr == bits)
    {
        return FailPass();
    }
    
    stbi_image_free(bits);

    return Pass();
}