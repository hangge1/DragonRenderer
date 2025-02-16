

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
	VertexArrayObject(VertexArrayObject&&) = delete;
    VertexArrayObject& operator=(const VertexArrayObject&) = delete;
	VertexArrayObject& operator=(VertexArrayObject&&) = delete;

	void BindVertexAttribute(uint32_t vertexAttrId, uint32_t vbo_id, size_t item_size, size_t stride, size_t offset);
	void BindVertexAttribute(uint32_t vertexAttrId, VertexAttrDescription desc);

	std::map<uint32_t, VertexAttrDescription> GetVertexAttrDescMap() const;

	//测试使用
    void Print() const;
	
private:
	//key:vertexAttrId - value:bindingDescription
	std::map<uint32_t, VertexAttrDescription> binding_map_;
};


#endif