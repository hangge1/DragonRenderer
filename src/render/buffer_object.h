
#ifndef _RENDER_BUFFER_OBJECT_H_
#define _RENDER_BUFFER_OBJECT_H_

#include <iostream>

class BufferObject
{
public:
    BufferObject() = default;
    ~BufferObject();
    BufferObject(const BufferObject&) = delete;
    BufferObject& operator=(const BufferObject&) = delete;

    void SetBuffer(void* buffer, size_t buffer_size);
    uint8_t* GetBuffer() const;
private:
    uint8_t* buffer_ { nullptr };
    size_t buffer_size_ { 0 };
};


#endif