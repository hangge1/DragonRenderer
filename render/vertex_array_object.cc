
#include "vertex_array_object.h"

#include <iostream>
#include <map>

void VertexArrayObject::BindVertexAttribute(uint32_t binding, uint32_t vbo_id, size_t item_size, size_t stride, size_t offset)
{
    auto it = binding_map_.find(binding);
    if(it == binding_map_.end())
    {
        binding_map_[binding] = VertexAttrDescription();
    }

    auto& desc = binding_map_[binding];
    desc.item_size = item_size;
    desc.vbo_id = vbo_id;
    desc.stride = stride;
    desc.offset = offset;
}

void VertexArrayObject::BindVertexAttribute(uint32_t vertexAttrId, VertexAttrDescription desc)
{
    BindVertexAttribute(vertexAttrId, desc.vbo_id, desc.item_size, desc.stride, desc.offset);
}

const std::map<uint32_t, VertexAttrDescription>& VertexArrayObject::GetVertexAttrDescMap() const
{
    return binding_map_;
}

void VertexArrayObject::Print() const
{
    std::cout << "VAO Attribute Map is:" << std::endl;
	for (auto& item : binding_map_) 
    {
		std::cout << "Binding ID:" << item.first << std::endl;
		std::cout << "----VBO ID:" << item.second.vbo_id << std::endl;
		std::cout << "----ItemSize:" << item.second.item_size << std::endl;
		std::cout << "----Stride:" << item.second.stride << std::endl;
		std::cout << "----Offset:" << item.second.offset << std::endl;
	}
}
