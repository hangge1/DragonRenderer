
#ifndef _APPLICATION_MODEL_H_
#define _APPLICATION_MODEL_H_

#include <map>
#include <string>
#include <vector>


#include "shader/lambert_light_shader.h"
#include "image.h"

#include "glm/glm.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"



class Renderer;
class Mesh;

class Model
{
public:
	Model(Renderer* renderer);
	~Model();

	Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    bool Read(const std::string& path);

	void Draw(LambertLightShader* shader);

	void SetModelMatrix(const glm::mat4& m);

private:
	void ProcessNode(Mesh* parent, aiNode* ainode, const aiScene* scene);
	
	Mesh* ProcessMesh(aiMesh* aimesh, const aiScene* scene);

	uint32_t ProcessTexture(const aiMaterial* material, const aiTextureType& type, const aiScene* scene);

	uint32_t CreateTexture(Image* image);

	static glm::mat4 GetMat4f(aiMatrix4x4 value);

private:
    Renderer* renderer_;
	std::string root_path_;
	Mesh* root_mesh_ { nullptr };
	std::vector<Mesh*> meshes_;
	//image path and textureID
	std::map<std::string, uint32_t> texture_cache_;
};

#endif