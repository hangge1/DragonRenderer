

#ifndef _RENDER_VERTEX_ARRAY_ObJECT_H_
#define _RENDER_VERTEX_ARRAY_ObJECT_H_

#include <map>

#include "pipeline_data.h"

class VertexArrayObject
{
public:
	VertexArrayObject() = default;
	~VertexArrayObject() = default;
    VertexArrayObject(const VertexArrayObject&) = delete;
    VertexArrayObject& operator=(const VertexArrayObject&) = delete;


	void Bind(uint32_t binding, uint32_t vbo_id, size_t item_size, size_t stride, size_t offset);

	std::map<uint32_t, BindingDescription> GetBindingMap() const;

    void Print() const;
private:
	//key:bindingId - value:bindingDescription
	std::map<uint32_t, BindingDescription> binding_map_;
};


#endif