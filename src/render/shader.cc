
#include "shader.h"

glm::vec4 Shader::getVector(
		const std::map<uint32_t, BindingDescription>& binding_map,
		const std::map<uint32_t, BufferObject*>& buffer_map,
		const uint32_t& attribute_location,
		const uint32_t& index)
{
    //1 取出本属性的Description
	auto binding_iter = binding_map.find(attribute_location);
	if (binding_iter == binding_map.end()) {
		assert(false);
	}

	auto bindingDescription = binding_iter->second;

	//2 取出本属性所在的vbo
	auto vbo_id = bindingDescription.vbo_id;
	auto vbo_iter = buffer_map.find(vbo_id);
	if (vbo_iter == buffer_map.end()) {
		assert(false);
	}

	BufferObject* vbo = vbo_iter->second;

	//3 计算数据在vbo中的偏移量
	uint32_t dataOffset = bindingDescription.stride * index + bindingDescription.offset;
	uint32_t dataSize = bindingDescription.item_size * sizeof(float);

	const uint8_t* buffer = vbo->GetBuffer() + dataOffset;

	glm::vec4 result;

	//拷贝出需要的数据，最多4个float
	memcpy(&result, buffer, dataSize);

	return result;
}

Color Shader::VectorToRGBA(const glm::vec4& v) 
{
    Color color;
    color.r = v.x * 255.0;
    color.g = v.y * 255.0;
    color.b = v.z * 255.0;
    color.a = v.w * 255.0;

    return color;
}