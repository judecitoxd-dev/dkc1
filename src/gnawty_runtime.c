#include "dk1/gnawty_runtime.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

typedef struct Dk1GnawtyCallbackCapture {
    Dk1GnawtyRuntime *runtime;
    size_t count;
} Dk1GnawtyCallbackCapture;

static int16_t wrapped_dx(uint16_t a, uint16_t b) {
    return (int16_t)(uint16_t)(a - b);
}

static int32_t absolute_i32(int32_t value) {
    return value < 0 ? -value : value;
}

static void capture_callback(size_t slot, Dk1ObjectPass pass,
                             uint32_t callback_pc, void *user_data) {
    Dk1GnawtyCallbackCapture *capture =
        (Dk1GnawtyCallbackCapture *)user_data;
    Dk1GnawtyRuntime *runtime;
    if (capture == NULL || capture->runtime == NULL ||
        slot != capture->runtime->slot ||
        pass != DK1_OBJECT_PASS_PRIMARY_NORMAL)
        return;
    runtime = capture->runtime;
    ++capture->count;
    ++runtime->scheduler_callbacks;
    runtime->callback_pc = callback_pc;
    runtime->callback_verified = callback_pc == DK1_GNAWTY_CALLBACK_PC;
}

static bool set_animation(Dk1GnawtyRuntime *runtime,
                          const Dk1RomImage *rom,
                          uint16_t animation_id) {
    uint16_t pointer;
    if (runtime == NULL || rom == NULL ||
        !dk1_animation_script_pointer(rom, animation_id, &pointer))
        return false;
    memset(&runtime->animation, 0, sizeof(runtime->animation));
    runtime->animation.animation_id = animation_id;
    runtime->animation.script_pointer = pointer;
    runtime->animation.speed = 0x0100;
    return true;
}

static bool support_near(const Dk1PlayerTerrainRuntime *terrain,
                         int32_t world_x, int32_t target_y,
                         int32_t tolerance,
                         Dk1TerrainSample *support) {
    Dk1TerrainSample sample;
    Dk1TerrainSample best = {0};
    int32_t best_delta = INT32_MAX;
    size_t row;
    if (terrain == NULL || !terrain->ready || support == NULL || world_x < 0)
        return false;
    for (row = 0u; row < DK1_TERRAIN_ROWS; ++row) {
        int32_t cell_y = (int32_t)row * DK1_TERRAIN_CELL_SIZE;
        int32_t delta;
        if (!dk1_rom_terrain_sample_cell(&terrain->view, world_x,
                                         cell_y, &sample))
            continue;
        delta = sample.floor_y - target_y;
        if (absolute_i32(delta) > tolerance)
            continue;
        if (absolute_i32(delta) < best_delta) {
            best = sample;
            best_delta = absolute_i32(delta);
        }
    }
    if (!best.hit)
        return false;
    *support = best;
    return true;
}

static bool body_wall(const Dk1PlayerTerrainRuntime *terrain,
                      int32_t leading_x, int32_t center_y) {
    Dk1TerrainSample sample;
    const int32_t probes[3] = {
        center_y - DK1_GNAWTY_HALF_HEIGHT + 2,
        center_y,
        center_y + DK1_GNAWTY_HALF_HEIGHT - 2
    };
    size_t i;
    if (terrain == NULL || !terrain->ready)
        return false;
    for (i = 0u; i < 3u; ++i) {
        if (dk1_rom_terrain_point_solid(&terrain->view, leading_x,
                                        probes[i], &sample))
            return true;
    }
    return false;
}

static bool overlaps_player(const Dk1GnawtyRuntime *runtime,
                            const Dk1PlayerPreviewRuntime *player) {
    int32_t dx;
    int32_t dy;
    if (runtime == NULL || player == NULL)
        return false;
    dx = wrapped_dx(runtime->motion.world_x, player->motion.world_x);
    dy = (int32_t)runtime->motion.world_y - player->motion.world_y;
    return absolute_i32(dx) <= DK1_GNAWTY_HALF_WIDTH + 7 &&
           absolute_i32(dy) <= DK1_GNAWTY_HALF_HEIGHT + 16;
}

static bool start_defeat(Dk1GnawtyRuntime *runtime,
                         const Dk1RomImage *rom,
                         Dk1PlayerPreviewRuntime *player) {
    if (!set_animation(runtime, rom, DK1_ANIMATION_GNAWTY_DEAD))
        return false;
    runtime->phase = DK1_GNAWTY_DEFEATED;
    runtime->defeat_timer = DK1_GNAWTY_DEFEAT_FRAMES;
    runtime->motion.velocity_x = 0;
    runtime->motion.velocity_y = 0;
    runtime->motion.subpixel_x = 0u;
    runtime->motion.subpixel_y = 0u;
    ++runtime->stomps;
    if (player != NULL) {
        player->motion.velocity_y = DK1_GNAWTY_STOMP_BOUNCE;
        player->motion.subpixel_y = 0u;
        player->airborne = true;
    }
    return true;
}

bool dk1_gnawty_spawn(Dk1GnawtyRuntime *runtime,
                       const Dk1RomImage *rom,
                       size_t slot,
                       uint16_t world_x,
                       int16_t world_y,
                       bool facing_left) {
    Dk1ObjectTypeDispatch dispatch;
    uint32_t callback_pc;
    uint32_t right;
    if (runtime == NULL || rom == NULL ||
        !dk1_object_slot_in_pool(slot, DK1_OBJECT_POOL_PRIMARY) ||
        !dk1_object_type_dispatch_get(DK1_OBJECT_TYPE_GNAWTY, &dispatch))
        return false;
    callback_pc = ((uint32_t)DK1_OBJECT_CALLBACK_BANK << 16u) |
                  dispatch.callback_address_bf;
    if (callback_pc != DK1_GNAWTY_CALLBACK_PC)
        return false;
    memset(runtime, 0, sizeof(*runtime));
    runtime->type_id = DK1_OBJECT_TYPE_GNAWTY;
    runtime->slot = slot;
    runtime->motion.world_x = world_x;
    runtime->motion.world_y = world_y;
    runtime->motion.velocity_x = facing_left ?
        (int16_t)-DK1_GNAWTY_WALK_SPEED : DK1_GNAWTY_WALK_SPEED;
    runtime->facing_left = facing_left;
    runtime->patrol_left = world_x > 96u ? (uint16_t)(world_x - 96u) : 0u;
    right = (uint32_t)world_x + 96u;
    runtime->patrol_right = right > UINT16_MAX ? UINT16_MAX : (uint16_t)right;
    runtime->scheduler.type_id[slot] = runtime->type_id;
    runtime->phase = DK1_GNAWTY_WALKING;
    runtime->active = true;
    return set_animation(runtime, rom, DK1_ANIMATION_GNAWTY_WALK);
}

bool dk1_gnawty_step(Dk1GnawtyRuntime *runtime,
                      const Dk1RomImage *rom,
                      const Dk1PlayerTerrainRuntime *terrain,
                      Dk1PlayerPreviewRuntime *player,
                      Dk1GnawtyStepResult *result) {
    Dk1GnawtyCallbackCapture capture;
    Dk1ObjectFrameResult frame_result;
    Dk1TerrainSample support;
    uint16_t old_x;
    int32_t leading_x;
    bool turn = false;
    if (runtime == NULL || rom == NULL || result == NULL || !runtime->active)
        return false;
    memset(result, 0, sizeof(*result));
    runtime->callback_verified = false;
    capture.runtime = runtime;
    capture.count = 0u;
    frame_result = dk1_object_run_frame(&runtime->scheduler,
                                         capture_callback, &capture);
    if (capture.count != 1u || frame_result.primary_callbacks != 1u ||
        !runtime->callback_verified)
        return false;

    ++runtime->frames;
    if (!dk1_object_animation_step(rom, &runtime->animation,
                                   &runtime->animation_result))
        return false;

    if (runtime->phase == DK1_GNAWTY_DEFEATED) {
        if (runtime->defeat_timer > 0u)
            --runtime->defeat_timer;
        if (runtime->defeat_timer == 0u) {
            runtime->active = false;
            runtime->scheduler.type_id[runtime->slot] = 0u;
        }
        result->render_requested = runtime->active;
        return true;
    }

    old_x = runtime->motion.world_x;
    runtime->motion.velocity_x = runtime->facing_left ?
        (int16_t)-DK1_GNAWTY_WALK_SPEED : DK1_GNAWTY_WALK_SPEED;
    dk1_player_motion_integrate_horizontal(&runtime->motion);
    leading_x = (int32_t)runtime->motion.world_x +
                (runtime->facing_left ? -DK1_GNAWTY_HALF_WIDTH :
                                        DK1_GNAWTY_HALF_WIDTH);

    if (body_wall(terrain, leading_x, runtime->motion.world_y)) {
        turn = true;
        result->wall_hit = true;
        ++runtime->wall_hits;
    } else if (terrain != NULL && terrain->ready &&
               !support_near(terrain,
                   leading_x + (runtime->facing_left ? -2 : 2),
                   runtime->motion.world_y - DK1_GNAWTY_HALF_HEIGHT,
                   14, &support)) {
        turn = true;
        result->ledge_hit = true;
        ++runtime->ledge_hits;
    } else if (runtime->motion.world_x <= runtime->patrol_left ||
               runtime->motion.world_x >= runtime->patrol_right) {
        turn = true;
    }

    if (turn) {
        runtime->motion.world_x = old_x;
        runtime->motion.subpixel_x = 0u;
        runtime->facing_left = !runtime->facing_left;
        ++runtime->turns;
        result->turned = true;
    }

    if (terrain != NULL && terrain->ready &&
        support_near(terrain, runtime->motion.world_x,
                     runtime->motion.world_y - DK1_GNAWTY_HALF_HEIGHT,
                     14, &support)) {
        runtime->motion.world_y =
            (int16_t)(support.floor_y + DK1_GNAWTY_HALF_HEIGHT);
        runtime->motion.subpixel_y = 0u;
    }

    if (player != NULL && overlaps_player(runtime, player)) {
        ++runtime->player_contacts;
        if (player->motion.velocity_y <= 0 &&
            player->motion.world_y >= runtime->motion.world_y + 4) {
            if (!start_defeat(runtime, rom, player))
                return false;
            result->stomped = true;
        } else {
            result->player_hurt = true;
        }
    }

    result->render_requested = runtime->active;
    return true;
}

bool dk1_gnawty_build_visual(Dk1GnawtyRuntime *runtime,
                              const Dk1RomImage *rom,
                              const Dk1VramImage *base_vram,
                              uint16_t camera_x,
                              uint16_t camera_y,
                              uint16_t vertical_origin,
                              uint8_t obsel) {
    Dk1ObjectScreenTransform transform;
    Dk1ObjectFrameOamStyle style;
    if (runtime == NULL || rom == NULL || base_vram == NULL ||
        !runtime->active || runtime->animation.frame == 0u)
        return false;
    memset(&transform, 0, sizeof(transform));
    memset(&style, 0, sizeof(style));
    transform.world_x = (int16_t)runtime->motion.world_x;
    transform.world_y = runtime->motion.world_y;
    transform.camera_x = (int16_t)camera_x;
    transform.vertical_origin = (int16_t)vertical_origin;
    transform.vertical_scroll = (int16_t)camera_y;
    transform.object_flags = runtime->facing_left ? 0x4000u : 0u;
    style.base_tile_number = 0x0040u;
    style.palette = 0u;
    style.priority = 3u;
    runtime->visual_ready = dk1_player_visual_build(
        rom, base_vram, runtime->animation.frame,
        transform, style, obsel,
        &runtime->oam, &runtime->vram, &runtime->visual);
    return runtime->visual_ready;
}
