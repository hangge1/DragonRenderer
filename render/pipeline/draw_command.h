#ifndef _RENDER_DRAW_COMMAND_H_
#define _RENDER_DRAW_COMMAND_H_

#include <cstdint>

#include "pipeline_data.h"

class BufferObject;
class VertexArrayObject;

struct DrawCommand
{
    DRAW_MODE draw_mode;
    uint32_t first { 0 };
    uint32_t count { 0 };
    const VertexArrayObject* vao { nullptr };
    const BufferObject* ebo { nullptr };
};

#endif
