#ifndef DK1_FRAME_REPLAY_H
#define DK1_FRAME_REPLAY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/scene_runtime.h"

#define DK1_REPLAY_MAX_FRAMES 4096u

typedef struct Dk1ReplayFrame {
    uint16_t held;
} Dk1ReplayFrame;

typedef struct Dk1FrameReplay {
    uint16_t level_id;
    Dk1SceneRuntime initial_runtime;
    Dk1ReplayFrame frames[DK1_REPLAY_MAX_FRAMES];
    size_t frame_count;
} Dk1FrameReplay;

void dk1_frame_replay_init(Dk1FrameReplay *replay, uint16_t level_id,
                           Dk1SceneRuntime initial_runtime);
bool dk1_frame_replay_append(Dk1FrameReplay *replay, uint16_t held);
bool dk1_frame_replay_run(const Dk1SceneMemory *scene,
                          const Dk1FrameReplay *replay,
                          Dk1SceneRuntime *runtime);
uint64_t dk1_frame_replay_signature(const Dk1FrameReplay *replay,
                                    const Dk1SceneRuntime *final_runtime);

#endif
