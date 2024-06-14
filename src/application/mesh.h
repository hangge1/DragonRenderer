
#ifndef _APPLICATION_MESH_H_
#define _APPLICATION_MESH_H_

#include <vector>

#include "glm/glm.hpp"
#include "shader/lambert_light_shader.h"

struct Vertex 
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

class Renderer;

class Mesh
{
public:
	Mesh(Renderer* renderer);
	~Mesh();
	Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void Init(const std::vector<Vertex>& vertices, const std::vector<unsigned int> indices, 
		      uint32_t diffuseTexture, const glm::mat4& localMatrix);
    void AddChild(Mesh* mesh);
	void Draw(const glm::mat4& preMatrix, LambertLightShader* shader);

    glm::mat4 GetLocalMatrix() const;
    void SetLocalMatrix(const glm::mat4& matrix);
private:
    Renderer* renderer_;

    uint32_t vao_ { 0 };
	uint32_t vbo_ { 0 };
	uint32_t ebo_ { 0 };
	uint32_t texture_ { 0 };
	uint32_t indices_count_ { 0 };

	glm::mat4 local_matrix_;

	std::vector<Mesh*> children_;
};

#endif