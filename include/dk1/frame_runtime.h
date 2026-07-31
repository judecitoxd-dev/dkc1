#ifndef DK1_FRAME_RUNTIME_H
#define DK1_FRAME_RUNTIME_H

#include <stdint.h>
#include "dk1/input.h"
#include "dk1/object_executor.h"
#include "dk1/ppu_scroll.h"
#include "dk1/render_queue.h"

typedef struct Dk1FrameRuntime {
    uint64_t frame_number;
    Dk1InputState input;
    Dk1ObjectWorld objects;
    Dk1Viewport viewport;
} Dk1FrameRuntime;

typedef struct Dk1FrameOutput {
    Dk1ObjectExecutionResult objects;
    Dk1PpuScrollState scroll;
    Dk1RenderQueue render_queue;
} Dk1FrameOutput;

Dk1FrameOutput dk1_frame_runtime_step(
    Dk1FrameRuntime *runtime,
    const Dk1HostCallbackRegistry *callbacks,
    uint16_t joy1,
    uint16_t joy2,
    uint16_t player_mode,
    uint16_t selection_index,
    uint16_t active_player_or_mode
);

#endif
