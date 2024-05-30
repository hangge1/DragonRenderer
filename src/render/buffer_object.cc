
#include "buffer_object.h"

BufferObject::~BufferObject()
{
    if(nullptr != buffer_)
    {
        delete[] buffer_;
        buffer_ = nullptr;
    }
}

void BufferObject::SetBuffer(void *buffer, size_t buffer_size)
{
    if(nullptr != buffer_ && buffer_size > buffer_size_)
    {
        delete[] buffer_;
        buffer_ = nullptr;
    }

    if(nullptr == buffer_)
    {
        buffer_ = new uint8_t[buffer_size];
        buffer_size_ = buffer_size;
    }

    memcpy(buffer_, buffer, buffer_size);
}

uint8_t *BufferObject::GetBuffer() const
{
    return buffer_;
}
