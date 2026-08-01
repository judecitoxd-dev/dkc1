#include "dk1/barrel_live_runtime.h"
#include <limits.h>
#include <string.h>

#define BARREL_HALF_WIDTH 8
#define BARREL_HALF_HEIGHT 8
#define BARREL_PICKUP_X 18
#define BARREL_PICKUP_Y 20
#define BARREL_HOLD_X 12
#define BARREL_HOLD_Y 8
#define BARREL_THROW_X 0x0300
#define BARREL_THROW_Y 0x0300
#define BARREL_GRAVITY ((int16_t)-0x0070)
#define BARREL_TERMINAL ((int16_t)-0x0600)

static bool common_type(uint16_t type_id) {
    return type_id == DK1_OBJECT_TYPE_STEEL_KEG ||
           type_id == DK1_OBJECT_TYPE_BARREL ||
           type_id == DK1_OBJECT_TYPE_ROPE_BARREL ||
           type_id == DK1_OBJECT_TYPE_DK_BARREL ||
           type_id == DK1_OBJECT_TYPE_TNT_BARREL;
}

static uint16_t idle_animation(uint16_t type_id) {
    switch (type_id) {
    case DK1_OBJECT_TYPE_STEEL_KEG: return DK1_ANIMATION_STEEL_KEG_IDLE;
    case DK1_OBJECT_TYPE_BARREL: return DK1_ANIMATION_BARREL_IDLE;
    case DK1_OBJECT_TYPE_ROPE_BARREL: return DK1_ANIMATION_ROPE_BARREL_IDLE;
    case DK1_OBJECT_TYPE_DK_BARREL: return DK1_ANIMATION_DK_BARREL_IDLE;
    case DK1_OBJECT_TYPE_TNT_BARREL: return DK1_ANIMATION_TNT_BARREL_IDLE;
    default: return 0u;
    }
}

static int16_t wrapped_dx(uint16_t a, uint16_t b) {
    return (int16_t)(uint16_t)(a - b);
}

static int32_t absolute_i32(int32_t value) {
    return value < 0 ? -value : value;
}

static bool overlaps_player(const Dk1BarrelLiveRuntime *runtime,
                            const Dk1BarrelLivePlayer *player) {
    int32_t dx;
    int32_t dy;
    if (runtime == NULL || player == NULL) return false;
    dx = wrapped_dx(runtime->motion.world_x, player->world_x);
    dy = (int32_t)runtime->motion.world_y - player->world_y;
    return absolute_i32(dx) <= BARREL_PICKUP_X &&
           absolute_i32(dy) <= BARREL_PICKUP_Y;
}

static void sync_object_from_motion(Dk1BarrelLiveRuntime *runtime) {
    runtime->object.world_x = runtime->motion.world_x;
    runtime->object.world_y = runtime->motion.world_y;
    runtime->object.velocity_x = runtime->motion.velocity_x;
    runtime->object.velocity_y = runtime->motion.velocity_y;
}

static void sync_motion_from_object(Dk1BarrelLiveRuntime *runtime) {
    runtime->motion.world_x = runtime->object.world_x;
    runtime->motion.world_y = runtime->object.world_y;
    runtime->motion.velocity_x = runtime->object.velocity_x;
    runtime->motion.velocity_y = runtime->object.velocity_y;
}

static void apply_barrel_gravity(Dk1PlayerMotion *motion) {
    int32_t next;
    if (motion == NULL) return;
    next = (int32_t)motion->velocity_y + BARREL_GRAVITY;
    if (next < BARREL_TERMINAL) next = BARREL_TERMINAL;
    if (next > INT16_MAX) next = INT16_MAX;
    motion->velocity_y = (int16_t)next;
}

static bool side_solid(Dk1BarrelLiveRuntime *runtime,
                       Dk1BarrelLiveTerrainOps terrain,
                       uint16_t old_x,
                       uint16_t *attributes) {
    int16_t dx;
    int32_t leading_x;
    int32_t probes[3];
    size_t i;
    if (terrain.solid_point == NULL) return false;
    dx = wrapped_dx(runtime->motion.world_x, old_x);
    if (dx == 0) return false;
    leading_x = (int32_t)runtime->motion.world_x +
                (dx > 0 ? BARREL_HALF_WIDTH : -BARREL_HALF_WIDTH);
    probes[0] = runtime->motion.world_y - BARREL_HALF_HEIGHT + 2;
    probes[1] = runtime->motion.world_y;
    probes[2] = runtime->motion.world_y + BARREL_HALF_HEIGHT - 2;
    for (i = 0u; i < 3u; ++i) {
        if (terrain.solid_point(terrain.user_data, leading_x, probes[i], attributes))
            return true;
    }
    return false;
}

static bool dispatch(Dk1BarrelLiveRuntime *runtime,
                     Dk1BarrelRuntimeInputs inputs,
                     Dk1BarrelRuntimeResult *result) {
    sync_object_from_motion(runtime);
    if (!dk1_barrel_runtime_step(&runtime->object, inputs, result)) return false;
    runtime->dispatcher_steps++;
    sync_motion_from_object(runtime);
    return true;
}

static void consume_destroy(Dk1BarrelLiveRuntime *runtime,
                            Dk1BarrelLiveResult *result) {
    if (!result->dispatcher.destroyed) return;
    runtime->active = false;
    runtime->phase = DK1_BARREL_LIVE_INACTIVE;
    runtime->motion.velocity_x = 0;
    runtime->motion.velocity_y = 0;
    runtime->object.velocity_x = 0;
    runtime->object.velocity_y = 0;
    runtime->breaks++;
    if (result->dispatcher.exploded) runtime->explosions++;
    if (result->dispatcher.released_kong) runtime->kong_releases++;
    result->destroyed = true;
    result->exploded = result->dispatcher.exploded;
    result->released_kong = result->dispatcher.released_kong;
}

bool dk1_barrel_live_spawn(Dk1BarrelLiveRuntime *runtime,
                           size_t slot,
                           uint16_t type_id,
                           uint16_t world_x,
                           int16_t world_y) {
    uint16_t animation;
    if (runtime == NULL || !common_type(type_id)) return false;
    animation = idle_animation(type_id);
    if (animation == 0u) return false;
    memset(runtime, 0, sizeof(*runtime));
    runtime->slot = slot;
    runtime->animation_id = animation;
    runtime->object.type_id = type_id;
    runtime->object.state = 0u;
    runtime->object.world_x = world_x;
    runtime->object.world_y = world_y;
    runtime->object.timer_14c5 = 0x0020u;
    runtime->motion.world_x = world_x;
    runtime->motion.world_y = world_y;
    runtime->phase = DK1_BARREL_LIVE_IDLE;
    runtime->active = true;
    return true;
}

bool dk1_barrel_live_step(Dk1BarrelLiveRuntime *runtime,
                          Dk1BarrelLivePlayer *player,
                          Dk1BarrelLiveTerrainOps terrain,
                          Dk1BarrelLiveResult *result) {
    Dk1BarrelRuntimeInputs inputs;
    uint16_t old_x;
    int16_t old_y;
    uint16_t contact_attributes = 0u;
    int16_t floor_y = 0;
    bool hit_wall;
    bool hit_floor = false;

    if (runtime == NULL || player == NULL || result == NULL || !runtime->active)
        return false;
    memset(result, 0, sizeof(*result));
    memset(&inputs, 0, sizeof(inputs));
    runtime->frames++;

    if (runtime->phase != DK1_BARREL_LIVE_HELD && player->pickup_pressed &&
        !player->holding_barrel && overlaps_player(runtime, player)) {
        runtime->phase = DK1_BARREL_LIVE_HELD;
        runtime->object.state = 2u;
        runtime->motion.velocity_x = 0;
        runtime->motion.velocity_y = 0;
        runtime->motion.subpixel_x = 0u;
        runtime->motion.subpixel_y = 0u;
        player->holding_barrel = true;
        runtime->pickups++;
        result->picked_up = true;
    }

    if (runtime->phase == DK1_BARREL_LIVE_HELD) {
        runtime->motion.world_x = (uint16_t)(player->world_x +
            (player->facing_left ? -BARREL_HOLD_X : BARREL_HOLD_X));
        runtime->motion.world_y = (int16_t)(player->world_y + BARREL_HOLD_Y);
        runtime->motion.velocity_x = 0;
        runtime->motion.velocity_y = 0;
        runtime->object.state = 2u;
        inputs.cdeb_carry = true;
        if (!dispatch(runtime, inputs, &result->dispatcher)) return false;
        if (player->throw_pressed) {
            runtime->phase = DK1_BARREL_LIVE_THROWN;
            runtime->object.state = 3u;
            runtime->object.timer_14c5 = 0x0020u;
            runtime->motion.velocity_x = player->facing_left ?
                (int16_t)-BARREL_THROW_X : (int16_t)BARREL_THROW_X;
            runtime->motion.velocity_y = BARREL_THROW_Y;
            runtime->motion.subpixel_x = 0u;
            runtime->motion.subpixel_y = 0u;
            sync_object_from_motion(runtime);
            player->holding_barrel = false;
            runtime->throws++;
            result->thrown = true;
        }
        result->render_requested = runtime->active;
        return true;
    }

    if (runtime->phase == DK1_BARREL_LIVE_IDLE) {
        runtime->object.state = 0u;
        inputs.cd8f_carry = true;
        if (!dispatch(runtime, inputs, &result->dispatcher)) return false;
        result->render_requested = runtime->active;
        return true;
    }

    old_x = runtime->motion.world_x;
    old_y = runtime->motion.world_y;
    runtime->object.state = 3u;
    if (!dispatch(runtime, inputs, &result->dispatcher)) return false;
    if (result->dispatcher.destroyed) {
        consume_destroy(runtime, result);
        return true;
    }

    if (runtime->phase == DK1_BARREL_LIVE_THROWN) {
        apply_barrel_gravity(&runtime->motion);
        dk1_player_motion_integrate(&runtime->motion);
    } else {
        dk1_player_motion_integrate_horizontal(&runtime->motion);
    }

    hit_wall = side_solid(runtime, terrain, old_x, &contact_attributes);
    if (hit_wall) {
        runtime->motion.world_x = old_x;
        runtime->motion.subpixel_x = 0u;
        runtime->wall_attributes = contact_attributes;
        runtime->wall_hits++;
        result->wall_hit = true;
        memset(&inputs, 0, sizeof(inputs));
        inputs.d23c_carry = true;
        if (!dispatch(runtime, inputs, &result->dispatcher)) return false;
        if (runtime->object.type_id == DK1_OBJECT_TYPE_STEEL_KEG &&
            !result->dispatcher.destroyed) {
            runtime->motion.velocity_x = (int16_t)(-(int32_t)runtime->motion.velocity_x);
            runtime->object.velocity_x = runtime->motion.velocity_x;
        }
        consume_destroy(runtime, result);
        result->render_requested = runtime->active;
        return true;
    }

    if (runtime->phase == DK1_BARREL_LIVE_THROWN &&
        runtime->motion.velocity_y <= 0 && terrain.floor_crossing != NULL) {
        hit_floor = terrain.floor_crossing(
            terrain.user_data, runtime->motion.world_x,
            (int16_t)(old_y - BARREL_HALF_HEIGHT),
            (int16_t)(runtime->motion.world_y - BARREL_HALF_HEIGHT),
            &floor_y, &contact_attributes);
    }
    if (hit_floor) {
        runtime->motion.world_y = (int16_t)(floor_y + BARREL_HALF_HEIGHT);
        runtime->motion.subpixel_y = 0u;
        runtime->motion.velocity_y = 0;
        runtime->floor_attributes = contact_attributes;
        runtime->landings++;
        result->landed = true;
        memset(&inputs, 0, sizeof(inputs));
        inputs.d324_carry = true;
        if (!dispatch(runtime, inputs, &result->dispatcher)) return false;
        if (!result->dispatcher.destroyed)
            runtime->phase = runtime->motion.velocity_x == 0 ?
                DK1_BARREL_LIVE_IDLE : DK1_BARREL_LIVE_ROLLING;
        consume_destroy(runtime, result);
    }

    sync_object_from_motion(runtime);
    result->render_requested = runtime->active;
    return true;
}
