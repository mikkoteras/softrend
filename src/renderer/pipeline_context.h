#ifndef PIPELINE_CONTEXT_H
#define PIPELINE_CONTEXT_H

#include "benchmark_frame.h"
#include "framebuffer.h"

// data required by a stage in the rendering pipeline in window::run()
struct pipeline_context {
public:
    pipeline_context(int w, int h) :
        frame(w, h) {
    }

    framebuffer frame;
    benchmark_frame frame_stats;
};

#endif
