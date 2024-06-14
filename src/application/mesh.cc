
#include "mesh.h"
#include "pipeline_data.h"

#include "renderer.h"

Mesh::Mesh(Renderer* renderer)
    : renderer_(renderer)
{

}

Mesh::~Mesh()
{
    if (vao_) 
    {
        renderer_->DeleteVertexArray(vao_);
	}

	if (vbo_) 
    {
		renderer_->DeleteBuffer(vbo_);
	}

	if (ebo_) 
    {
		renderer_->DeleteBuffer(ebo_);
	}
}

void Mesh::Init(const std::vector<Vertex>& vertices, const std::vector<unsigned int> indices, 
            uint32_t diffuse_texture, const glm::mat4& local_matrix)
{
	if (vertices.empty() || indices.empty()) 
    {
		return;
	}

    //至少作为一个节点
	local_matrix_ = local_matrix;
	texture_ = diffuse_texture;

	vao_ = renderer_->GenVertexArray();
	vbo_ = renderer_->GenBuffer();
	ebo_ = renderer_->GenBuffer();

	renderer_->BindBuffer(ARRAY_BUFFER, vbo_);
	renderer_->BufferData(ARRAY_BUFFER, vertices.size() * sizeof(Vertex), (void*) & vertices[0]);

	renderer_->BindBuffer(ELEMENT_ARRAY_BUFFER, ebo_);
	renderer_->BufferData(ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), (void*)&indices[0]);
	indices_count_ = indices.size();

	renderer_->BindVertexArray(vao_);
	renderer_->VertexAttributePointer(0, 3, sizeof(Vertex), 0);
	renderer_->VertexAttributePointer(1, 3, sizeof(Vertex), sizeof(float) * 3);
	renderer_->VertexAttributePointer(2, 2, sizeof(Vertex), sizeof(float) * 6);
	renderer_->BindVertexArray(0);
}

void Mesh::AddChild(Mesh* mesh)
{
    children_.push_back(mesh);
}

void Mesh::Draw(const glm::mat4& preMatrix, LambertLightShader* shader)
{
    auto modelMatrix = preMatrix * local_matrix_;

	if (vao_) 
    {
		shader->model_matrix = modelMatrix;
		if (texture_) 
        {
			shader->diffuse_texture = texture_;
		}

		renderer_->BindVertexArray(vao_);
		renderer_->BindBuffer(ELEMENT_ARRAY_BUFFER, ebo_);
		renderer_->DrawElement(DRAW_TRIANGLES, 0, indices_count_);
		renderer_->BindVertexArray(0);
	}

	for (auto mesh : children_) 
    {
		mesh->Draw(modelMatrix, shader);
	}
}

glm::mat4 Mesh::GetLocalMatrix() const
{
    return local_matrix_;
}

void Mesh::SetLocalMatrix(const glm::mat4& matrix)
{
    local_matrix_ = matrix;
}