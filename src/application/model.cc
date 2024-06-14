
#include "model.h"

#include <map>
#include <string>
#include <vector>

#include "mesh.h"
#include "image.h"

#include "pipeline_data.h"
#include "renderer.h"

#include "glm/glm.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

Model::Model(Renderer* renderer)
    : renderer_(renderer) 
{

}

Model::~Model() 
{
	for (auto& mesh : meshes_) 
    {
		delete mesh;
	}

	for(auto& iter : texture_cache_) 
    {
		renderer_->DeleteTexture(iter.second);
	}
}

bool Model::Read(const std::string& path) 
{
	//assets/model/bag/bag.obj
	//assets/model/bag
	std::size_t lastIndex = path.find_last_of("//");
	root_path_ = path.substr(0, lastIndex + 1);

	//开始进行读取
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		//std::cout << "Error:model read fail!" << std::endl;
		return false;
	}

	root_mesh_ = new Mesh(renderer_);
    root_mesh_->Init({}, {}, 0, glm::mat4());
	ProcessNode(root_mesh_, scene->mRootNode, scene);

    return true;
}

void Model::Draw(LambertLightShader* shader) 
{
	root_mesh_->Draw(glm::mat4(), shader);
}

void Model::SetModelMatrix(const glm::mat4& m) 
{
    root_mesh_->SetLocalMatrix(m);
}

void Model::ProcessNode(Mesh* parent, aiNode* ainode, const aiScene* scene) 
{
	//创建新节点
	auto node = new Mesh(renderer_);
    node->Init({}, {}, 0, glm::mat4());
	parent->AddChild(node);
    node->SetLocalMatrix(GetMat4f(ainode->mTransformation));

	// 处理节点所有的网格（如果有的话）,挂载到本node下
	for (unsigned int i = 0; i < ainode->mNumMeshes; i++)
	{
		aiMesh* aimesh = scene->mMeshes[ainode->mMeshes[i]];
		auto mesh = ProcessMesh(aimesh, scene);
		meshes_.push_back(mesh);
		node->AddChild(mesh);
	}

	// 接下来对它的子节点重复这一过程
	for (unsigned int i = 0; i < ainode->mNumChildren; i++)
	{
		ProcessNode(node, ainode->mChildren[i], scene);
	}
}

Mesh* Model::ProcessMesh(aiMesh* aimesh, const aiScene* scene) 
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	uint32_t diffuseTexture = 0;

	for (unsigned int i = 0; i < aimesh->mNumVertices; i++)
	{
		Vertex vertex;
		// 处理顶点位置、法线和纹理坐标
		glm::vec3 vector;
		vector.x = aimesh->mVertices[i].x;
		vector.y = aimesh->mVertices[i].y;
		vector.z = aimesh->mVertices[i].z;
		vertex.position = vector;

		vector.x = aimesh->mNormals[i].x;
		vector.y = aimesh->mNormals[i].y;
		vector.z = aimesh->mNormals[i].z;
		vertex.normal = vector;

		if (aimesh->mTextureCoords[0]) // 网格是否有纹理坐标？
		{
			glm::vec2 vec;
			vec.x = aimesh->mTextureCoords[0][i].x;
			vec.y = aimesh->mTextureCoords[0][i].y;
			vertex.texCoords = vec;
		}
		else
        {
			vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }
		vertices.push_back(vertex);
	}
	// 处理索引
	for (unsigned int i = 0; i < aimesh->mNumFaces; i++)
	{
		aiFace face = aimesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	if (aimesh->mMaterialIndex >= 0)
	{
		//取出材质
		aiMaterial* material = scene->mMaterials[aimesh->mMaterialIndex];
		diffuseTexture = ProcessTexture(material, aiTextureType_DIFFUSE, scene);
	}

	//其上方已经拥有了parentNode，不需要自己的localMatrix
    Mesh* result = new Mesh(renderer_);
    result->Init(vertices, indices, diffuseTexture, glm::mat4());
	return result;
}

uint32_t Model::ProcessTexture(const aiMaterial* material, const aiTextureType& type,const aiScene* scene) 
{
	//for texture
	aiString aiPath;
	Image* image = nullptr;

	material->Get(AI_MATKEY_TEXTURE(type, 0), aiPath);

	if (!aiPath.length) 
    {
		return 0;
	}

	//先检查缓存是否有纹理
	auto iter = texture_cache_.find(std::string(aiPath.C_Str()));
	if (iter != texture_cache_.end()) 
    {
		return iter->second;
	}

	//部分模型在导出的时候，会把纹理图片打包到比如fbx格式当中，被打包到模型里面的图片，称为embeddedTexture 
	const aiTexture* assimpTexture = scene->GetEmbeddedTexture(aiPath.C_Str());
    uint32_t texture = 0;

	if (assimpTexture) 
    {
		//如果确实图片打包在了模型内部，则上述代码获取到的aiTexture里面就含有了图片数据
		unsigned char* dataIn = reinterpret_cast<unsigned char*>(assimpTexture->pcData);
		uint32_t widthIn = assimpTexture->mWidth;
		uint32_t heightIn = assimpTexture->mHeight;
		std::string path = aiPath.C_Str();
		Image* image = Image::CreateImageFromMemory(path, dataIn, widthIn, heightIn);
		
		texture = CreateTexture(image);

		Image::destroyImage(image);
	}
	else 
    {
		std::string fullPath = root_path_ + aiPath.C_Str();
		Image* image = Image::createImage(fullPath);

		texture = CreateTexture(image);

		Image::destroyImage(image);
	}

	texture_cache_.insert(std::make_pair(aiPath.C_Str(), texture));

	return texture;
}

uint32_t Model::CreateTexture(Image* image) 
{
	uint32_t texture = renderer_->GenTexture();
	renderer_->BindTexture(texture);
	renderer_->TexImage2D(image->get_width(), image->get_height(), image->get_data());
	renderer_->TexParameter(TEXTURE_FILTER, TEXTURE_FILTER_LINEAR);
	renderer_->TexParameter(TEXTURE_WRAP_U, TEXTURE_WRAP_REPEAT);
	renderer_->TexParameter(TEXTURE_WRAP_V, TEXTURE_WRAP_REPEAT);
	renderer_->BindTexture(0);

	return texture;
}


glm::mat4 Model::GetMat4f(aiMatrix4x4 value) 
{
	glm::mat4 to 
    {
        value.a1, value.a2, value.a3, value.a4,
		value.b1, value.b2, value.b3, value.b4,
		value.c1, value.c2, value.c3, value.c4,
		value.d1, value.d2, value.d3, value.d4
    };

	return glm::transpose(to);
}