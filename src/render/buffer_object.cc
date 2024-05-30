
#include "buffer_object.h"

void BufferObject::SetBuffer(uint8_t *buffer, size_t buffer_size)
{
    buffer_ = buffer;
    buffer_size_ = buffer_size;
}

uint8_t *BufferObject::GetBuffer() const
{
    return buffer_;
}
