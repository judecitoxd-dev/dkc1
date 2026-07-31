#include "dk1/player_preview_runtime.h"
#include <string.h>

void dk1_player_preview_init(Dk1PlayerPreviewRuntime *runtime,
                             int16_t world_x, int16_t world_y) {
    if (runtime == 0) return;
    memset(runtime, 0, sizeof(*runtime));
    runtime->motion.world_x = world_x;
    runtime->motion.world_y = world_y;
    runtime->floor_y = world_y;
    runtime->frame_id = 0x0330u;
}

void dk1_player_preview_step(Dk1PlayerPreviewRuntime *runtime,
                             uint16_t held, uint16_t pressed) {
    bool left;
    bool right;
    if (runtime == 0) return;
    left = (held & DK1_PLAYER_PREVIEW_LEFT) != 0u;
    right = (held & DK1_PLAYER_PREVIEW_RIGHT) != 0u;
    if (left && !right) {
        runtime->motion.target_velocity_x = (int16_t)-0x0180;
        runtime->facing_left = true;
    } else if (right && !left) {
        runtime->motion.target_velocity_x = 0x0180;
        runtime->facing_left = false;
    } else {
        runtime->motion.target_velocity_x = 0;
    }
    dk1_player_motion_approach_horizontal(&runtime->motion, 0u);

    if ((pressed & DK1_PLAYER_PREVIEW_JUMP) != 0u && !runtime->airborne) {
        runtime->motion.velocity_y = 0x0500;
        runtime->airborne = true;
        runtime->jumps++;
    }
    if (runtime->airborne) dk1_player_motion_apply_gravity(&runtime->motion, false, 0);
    dk1_player_motion_integrate(&runtime->motion);

    if (runtime->motion.world_y <= runtime->floor_y && runtime->motion.velocity_y <= 0) {
        runtime->motion.world_y = runtime->floor_y;
        runtime->motion.subpixel_y = 0u;
        runtime->motion.velocity_y = 0;
        runtime->airborne = false;
    }
    runtime->frames++;
}
