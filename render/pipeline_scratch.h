#ifndef DRAGON_RENDER_PIPELINE_SCRATCH_H_
#define DRAGON_RENDER_PIPELINE_SCRATCH_H_

#include <vector>

#include "pipeline_data.h"

struct PipelineScratch
{
    std::vector<VsOutput> vertex_outputs;
    std::vector<VsOutput> clip_outputs;
    std::vector<VsOutput> cull_outputs;
    std::vector<VsOutput> raster_outputs;

    void ResetForDraw(size_t vertex_count_hint)
    {
        vertex_outputs.clear();
        clip_outputs.clear();
        cull_outputs.clear();
        raster_outputs.clear();

        vertex_outputs.reserve(vertex_count_hint);
        clip_outputs.reserve(vertex_count_hint);
        cull_outputs.reserve(vertex_count_hint);
        raster_outputs.reserve(vertex_count_hint);
    }
};

#endif
