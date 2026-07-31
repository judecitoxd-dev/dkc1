#include "dk1/preview_actor.h"

static int32_t approach_zero(int32_t value, int32_t amount) {
    if (value > 0) return value > amount ? value - amount : 0;
    if (value < 0) return value < -amount ? value + amount : 0;
    return 0;
}

void dk1_preview_actor_step(Dk1PreviewActor *actor, Dk1PreviewActorConfig config, const Dk1InputState *input, const Dk1TerrainLayout *terrain) {
    Dk1TerrainSample floor = {0};
    const bool left = dk1_input_held(input, DK1_BUTTON_LEFT);
    const bool right = dk1_input_held(input, DK1_BUTTON_RIGHT);
    int32_t previous_bottom = 0;
    int32_t next_bottom = 0;
    if (actor == NULL) return;
    if (left != right) {
        actor->velocity_x += left ? -config.acceleration : config.acceleration;
        if (actor->velocity_x > config.maximum_speed) actor->velocity_x = config.maximum_speed;
        if (actor->velocity_x < -config.maximum_speed) actor->velocity_x = -config.maximum_speed;
    } else {
        actor->velocity_x = approach_zero(actor->velocity_x, config.friction);
    }
    if (actor->grounded && dk1_input_pressed(input, DK1_BUTTON_B)) {
        actor->velocity_y = config.jump_velocity;
        actor->grounded = false;
    }
    actor->x += actor->velocity_x;
    previous_bottom = actor->y + actor->height;
    actor->velocity_y += config.gravity;
    actor->y += actor->velocity_y;
    next_bottom = actor->y + actor->height;
    actor->grounded = false;
    if (actor->velocity_y >= 0 && terrain != NULL && dk1_terrain_find_floor(terrain, actor->x, previous_bottom, 2u, &floor) && previous_bottom <= floor.floor_y && next_bottom >= floor.floor_y) {
        actor->y = floor.floor_y - actor->height;
        actor->velocity_y = 0;
        actor->grounded = true;
    }
}
