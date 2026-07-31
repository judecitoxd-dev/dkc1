#include <assert.h>
#include "dk1/player_preview_runtime.h"

int main(void) {
    Dk1PlayerPreviewRuntime runtime;
    unsigned i;
    dk1_player_preview_init(&runtime, 100, 40);
    assert(runtime.motion.world_x == 100 && runtime.motion.world_y == 40);
    for (i = 0u; i < 8u; ++i)
        dk1_player_preview_step(&runtime, DK1_PLAYER_PREVIEW_RIGHT, 0u);
    assert(runtime.motion.world_x > 100);
    assert(runtime.motion.velocity_x > 0);
    assert(!runtime.facing_left);

    dk1_player_preview_step(&runtime, DK1_PLAYER_PREVIEW_JUMP,
                            DK1_PLAYER_PREVIEW_JUMP);
    assert(runtime.airborne && runtime.jumps == 1u);
    assert(runtime.motion.world_y > runtime.floor_y);
    for (i = 0u; i < 200u && runtime.airborne; ++i)
        dk1_player_preview_step(&runtime, 0u, 0u);
    assert(!runtime.airborne);
    assert(runtime.motion.world_y == runtime.floor_y);
    assert(runtime.motion.velocity_y == 0);

    for (i = 0u; i < 40u; ++i)
        dk1_player_preview_step(&runtime, DK1_PLAYER_PREVIEW_LEFT, 0u);
    assert(runtime.motion.velocity_x < 0);
    assert(runtime.facing_left);
    return 0;
}
