
#include "vertex_array_object.h"

#include <map>

void VertexArrayObject::Bind(uint32_t binding, uint32_t vbo_id, size_t item_size, size_t stride, size_t offset)
{
    auto it = binding_map_.find(binding);
    if(it == binding_map_.end())
    {
        binding_map_[binding] = BindingDescription();
    }

    auto& desc = binding_map_[binding];
    desc.item_size = item_size;
    desc.vbo_id = vbo_id;
    desc.stride = stride;
    desc.offset = offset;

}

std::map<uint32_t, BindingDescription> VertexArrayObject::GetBindingMap() const
{
    return binding_map_;
}
