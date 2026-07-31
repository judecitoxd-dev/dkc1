#ifndef DK1_PREVIEW_ACTOR_H
#define DK1_PREVIEW_ACTOR_H

#include <stdbool.h>
#include <stdint.h>
#include "dk1/input.h"
#include "dk1/terrain.h"

typedef struct Dk1PreviewActor {
    int32_t x;
    int32_t y;
    int32_t velocity_x;
    int32_t velocity_y;
    int32_t half_width;
    int32_t height;
    bool grounded;
} Dk1PreviewActor;

typedef struct Dk1PreviewActorConfig {
    int32_t acceleration;
    int32_t maximum_speed;
    int32_t friction;
    int32_t gravity;
    int32_t jump_velocity;
} Dk1PreviewActorConfig;

void dk1_preview_actor_step(Dk1PreviewActor *actor, Dk1PreviewActorConfig config, const Dk1InputState *input, const Dk1TerrainLayout *terrain);

#endif
