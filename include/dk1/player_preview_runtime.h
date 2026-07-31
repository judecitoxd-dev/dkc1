#ifndef DK1_PLAYER_PREVIEW_RUNTIME_H
#define DK1_PLAYER_PREVIEW_RUNTIME_H

#include <stdbool.h>
#include <stdint.h>
#include "dk1/player_motion.h"

#define DK1_PLAYER_PREVIEW_LEFT  0x0020u
#define DK1_PLAYER_PREVIEW_RIGHT 0x0010u
#define DK1_PLAYER_PREVIEW_JUMP  0x8000u

typedef struct Dk1PlayerPreviewRuntime {
    Dk1PlayerMotion motion;
    int16_t floor_y;
    uint16_t frame_id;
    uint64_t frames;
    uint32_t jumps;
    bool airborne;
    bool facing_left;
} Dk1PlayerPreviewRuntime;

void dk1_player_preview_init(Dk1PlayerPreviewRuntime *runtime,
                             int16_t world_x, int16_t world_y);
void dk1_player_preview_step(Dk1PlayerPreviewRuntime *runtime,
                             uint16_t held, uint16_t pressed);

#endif
