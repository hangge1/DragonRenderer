
#include <iostream>
#include <string>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

int Pass()
{
    std::cout << "thirdparts library assimp Pass" << std::endl;
    return 0;
}

int FailPass()
{
     std::cout << "thirdparts library assimp Can't Pass" << std::endl;
    return -1;
}


int main(int argc, char** argv)
{

    //开始进行读取
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		ASSETS_PATH "/model/dinosaur/source/Rampaging T-Rex.glb",
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		return FailPass();
	}

    return Pass();
}