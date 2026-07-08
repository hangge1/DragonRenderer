#ifndef _RENDER_RUNTIME_FRAME_STATS_H_
#define _RENDER_RUNTIME_FRAME_STATS_H_

#include <cstdint>

struct FrameStats
{
    double frame_ms { 0.0 };
    double update_ms { 0.0 };
    double render_ms { 0.0 };
    double present_ms { 0.0 };

    uint32_t draw_calls { 0 };
    uint32_t input_triangles { 0 };
    uint32_t clipped_triangles { 0 };
    uint32_t rasterized_fragments { 0 };
    uint32_t shaded_fragments { 0 };
    uint32_t depth_rejected_fragments { 0 };

    void Reset()
    {
        *this = {};
    }
};

#endif
