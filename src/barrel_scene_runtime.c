#include "dk1/barrel_scene_runtime.h"
#include <string.h>
#include "dk1/host_input.h"
#include "dk1/rom_terrain.h"

#define BARREL_VISUAL_TILE_BASE 0x0080u
#define BARREL_VISUAL_PALETTE 0u

static uint16_t phase_animation(const Dk1BarrelLiveRuntime *live) {
    uint16_t base;
    if (live == NULL) return 0u;
    base = live->animation_id;
    switch (live->phase) {
    case DK1_BARREL_LIVE_HELD:
        return (uint16_t)(base + 6u);
    case DK1_BARREL_LIVE_THROWN:
    case DK1_BARREL_LIVE_ROLLING:
        return (uint16_t)(base + 12u);
    default:
        return base;
    }
}

static bool select_animation(Dk1BarrelSceneRuntime *runtime,
                             const Dk1RomImage *rom,
                             uint16_t animation_id) {
    uint16_t pointer;
    if (runtime == NULL || rom == NULL || animation_id == 0u) return false;
    if (runtime->selected_animation == animation_id &&
        runtime->animation.script_pointer >= 0x8000u) return true;
    if (!dk1_animation_script_pointer(rom, animation_id, &pointer)) return false;
    memset(&runtime->animation, 0, sizeof(runtime->animation));
    runtime->animation.animation_id = animation_id;
    runtime->animation.script_pointer = pointer;
    runtime->animation.speed = 0x0100;
    runtime->selected_animation = animation_id;
    runtime->animation_resets++;
    return true;
}

static bool advance_animation(Dk1BarrelSceneRuntime *runtime,
                              const Dk1RomImage *rom) {
    unsigned attempts;
    uint16_t desired;
    if (runtime == NULL || rom == NULL || !runtime->live.active) return false;
    desired = phase_animation(&runtime->live);
    if (!select_animation(runtime, rom, desired)) return false;
    for (attempts = 0u; attempts < 4u; ++attempts) {
        if (!dk1_object_animation_step(rom, &runtime->animation,
                                       &runtime->animation_result)) return false;
        if (runtime->animation_result.status == DK1_ANIMATION_EXTERNAL_CALL) {
            runtime->animation_external_calls++;
            continue;
        }
        if (runtime->animation_result.status == DK1_ANIMATION_LINK_REQUIRED ||
            runtime->animation_result.status == DK1_ANIMATION_UNSUPPORTED_COMMAND ||
            runtime->animation_result.status == DK1_ANIMATION_INVALID_DATA)
            return false;
        return runtime->animation.frame != 0u;
    }
    return runtime->animation.frame != 0u;
}

static bool terrain_floor_crossing(void *user_data,
                                   uint16_t world_x,
                                   int16_t old_bottom_y,
                                   int16_t new_bottom_y,
                                   int16_t *floor_y,
                                   uint16_t *attributes) {
    Dk1PlayerTerrainRuntime *terrain = (Dk1PlayerTerrainRuntime *)user_data;
    Dk1TerrainSample sample;
    if (terrain == NULL || floor_y == NULL || attributes == NULL ||
        !terrain->ready || new_bottom_y > old_bottom_y)
        return false;
    if (!dk1_rom_terrain_find_floor(&terrain->view, world_x,
                                    old_bottom_y, 2u, &sample)) return false;
    if (sample.floor_y > old_bottom_y || sample.floor_y < new_bottom_y)
        return false;
    *floor_y = (int16_t)sample.floor_y;
    *attributes = sample.attributes;
    return true;
}

static bool terrain_solid_point(void *user_data,
                                int32_t world_x,
                                int32_t world_y,
                                uint16_t *attributes) {
    Dk1PlayerTerrainRuntime *terrain = (Dk1PlayerTerrainRuntime *)user_data;
    Dk1TerrainSample sample;
    if (terrain == NULL || attributes == NULL || !terrain->ready) return false;
    if (!dk1_rom_terrain_point_solid(&terrain->view, world_x, world_y, &sample))
        return false;
    *attributes = sample.attributes;
    return true;
}

bool dk1_barrel_scene_spawn(Dk1BarrelSceneRuntime *runtime,
                            const Dk1RomImage *rom,
                            size_t slot,
                            uint16_t type_id,
                            uint16_t world_x,
                            int16_t world_y) {
    if (runtime == NULL || rom == NULL) return false;
    memset(runtime, 0, sizeof(*runtime));
    if (!dk1_barrel_live_spawn(&runtime->live, slot, type_id, world_x, world_y))
        return false;
    if (!select_animation(runtime, rom, runtime->live.animation_id)) return false;
    runtime->ready = true;
    return advance_animation(runtime, rom);
}

bool dk1_barrel_scene_step(Dk1BarrelSceneRuntime *runtime,
                           const Dk1RomImage *rom,
                           Dk1PlayerTerrainRuntime *terrain,
                           const Dk1PlayerPreviewRuntime *player,
                           uint16_t pressed,
                           Dk1BarrelLiveResult *result) {
    Dk1BarrelLivePlayer live_player;
    Dk1BarrelLiveTerrainOps terrain_ops;
    bool action;
    bool already_held;
    if (runtime == NULL || rom == NULL || player == NULL || result == NULL ||
        !runtime->ready || !runtime->live.active)
        return false;
    memset(&live_player, 0, sizeof(live_player));
    memset(&terrain_ops, 0, sizeof(terrain_ops));
    already_held = runtime->live.phase == DK1_BARREL_LIVE_HELD;
    action = (pressed & DK1_HOST_BUTTON_Y) != 0u;
    live_player.world_x = player->motion.world_x;
    live_player.world_y = player->motion.world_y;
    live_player.velocity_x = player->motion.velocity_x;
    live_player.velocity_y = player->motion.velocity_y;
    live_player.facing_left = player->facing_left;
    live_player.pickup_pressed = action && !already_held;
    live_player.throw_pressed = action && already_held;
    live_player.holding_barrel = already_held;
    if (terrain != NULL && terrain->ready) {
        terrain_ops.user_data = terrain;
        terrain_ops.floor_crossing = terrain_floor_crossing;
        terrain_ops.solid_point = terrain_solid_point;
    }
    if (!dk1_barrel_live_step(&runtime->live, &live_player, terrain_ops, result))
        return false;
    runtime->visual_ready = false;
    if (!runtime->live.active) return true;
    return advance_animation(runtime, rom);
}

bool dk1_barrel_scene_build_visual(Dk1BarrelSceneRuntime *runtime,
                                   const Dk1RomImage *rom,
                                   const Dk1VramImage *base_vram,
                                   uint16_t camera_x,
                                   uint16_t camera_y,
                                   uint16_t vertical_origin,
                                   uint8_t obsel) {
    Dk1ObjectScreenTransform transform;
    Dk1ObjectFrameOamStyle style;
    if (runtime == NULL || rom == NULL || base_vram == NULL ||
        !runtime->ready || !runtime->live.active || runtime->animation.frame == 0u)
        return false;
    memset(&transform, 0, sizeof(transform));
    transform.world_x = (int16_t)runtime->live.motion.world_x;
    transform.world_y = runtime->live.motion.world_y;
    transform.camera_x = (int16_t)camera_x;
    transform.vertical_origin = (int16_t)vertical_origin;
    transform.vertical_scroll = (int16_t)camera_y;
    transform.object_flags = runtime->live.motion.velocity_x < 0 ? 0x4000u : 0u;
    memset(&style, 0, sizeof(style));
    style.base_tile_number = BARREL_VISUAL_TILE_BASE;
    style.palette = BARREL_VISUAL_PALETTE;
    style.priority = 3u;
    runtime->visual_ready = dk1_player_visual_build(
        rom, base_vram, runtime->animation.frame, transform, style, obsel,
        &runtime->oam, &runtime->vram, &runtime->visual);
    return runtime->visual_ready;
}
