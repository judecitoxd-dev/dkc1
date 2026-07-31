#include "dk1/frame_replay.h"
#include <string.h>
#include "dk1/scene_signature.h"

void dk1_frame_replay_init(Dk1FrameReplay *replay, uint16_t level_id,
                           Dk1SceneRuntime initial_runtime) {
    if (replay == NULL) return;
    memset(replay, 0, sizeof(*replay));
    replay->level_id = level_id;
    replay->initial_runtime = initial_runtime;
}

bool dk1_frame_replay_append(Dk1FrameReplay *replay, uint16_t held) {
    if (replay == NULL || replay->frame_count >= DK1_REPLAY_MAX_FRAMES) return false;
    replay->frames[replay->frame_count++].held = held;
    return true;
}

bool dk1_frame_replay_run(const Dk1SceneMemory *scene,
                          const Dk1FrameReplay *replay,
                          Dk1SceneRuntime *runtime) {
    size_t i;
    if (scene == NULL || replay == NULL || runtime == NULL) return false;
    *runtime = replay->initial_runtime;
    for (i = 0; i < replay->frame_count; ++i) {
        if (!dk1_scene_runtime_step(scene, replay->frames[i].held, runtime)) return false;
    }
    return true;
}

uint64_t dk1_frame_replay_signature(const Dk1FrameReplay *replay,
                                    const Dk1SceneRuntime *final_runtime) {
    uint64_t hash = 0;
    if (replay == NULL || final_runtime == NULL) return 0;
    hash = dk1_fnv1a64(&replay->level_id, sizeof(replay->level_id), hash);
    hash = dk1_fnv1a64(&replay->initial_runtime, sizeof(replay->initial_runtime), hash);
    hash = dk1_fnv1a64(replay->frames, replay->frame_count * sizeof(replay->frames[0]), hash);
    return dk1_fnv1a64(final_runtime, sizeof(*final_runtime), hash);
}
