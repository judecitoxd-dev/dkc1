#include "dk1/player_terrain_runtime.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

static bool best_support(const Dk1PlayerTerrainRuntime *r, uint16_t x,
                         int32_t target, int32_t below, int32_t above,
                         Dk1TerrainSample *out) {
    static const int16_t dx[3] = {-7, 0, 7};
    Dk1TerrainSample sample, best = {0};
    int32_t best_delta = INT32_MAX;
    size_t i, row;
    if (r == NULL || !r->ready || out == NULL) return false;
    for (i = 0u; i < 3u; ++i) {
        int32_t sample_x = (int32_t)x + dx[i];
        if (sample_x < 0) continue;
        for (row = 0u; row < DK1_TERRAIN_ROWS; ++row) {
            int32_t cell_y = (int32_t)row * DK1_TERRAIN_CELL_SIZE;
            int32_t delta;
            if (!dk1_rom_terrain_sample_cell(&r->view, sample_x, cell_y, &sample)) continue;
            if (sample.floor_y < target - below || sample.floor_y > target + above) continue;
            delta = abs((int)(sample.floor_y - target));
            if (delta < best_delta ||
                (delta == best_delta && sample.floor_y > best.floor_y)) {
                best = sample;
                best_delta = delta;
            }
        }
    }
    if (!best.hit) return false;
    *out = best;
    return true;
}

static bool body_side_solid(const Dk1PlayerTerrainRuntime *r,
                            int32_t x, int32_t center_y,
                            Dk1TerrainSample *contact) {
    const int32_t foot = center_y - r->foot_offset;
    const int32_t head = center_y + r->foot_offset;
    const int32_t probes[3] = {foot + 3, center_y, head - 3};
    Dk1TerrainSample sample;
    size_t i;
    for (i = 0u; i < 3u; ++i) {
        if (dk1_rom_terrain_point_solid(&r->view, x, probes[i], &sample)) {
            if (contact != NULL) *contact = sample;
            return true;
        }
    }
    return false;
}

static bool head_solid(const Dk1PlayerTerrainRuntime *r,
                       uint16_t center_x, int32_t center_y,
                       Dk1TerrainSample *contact) {
    const int32_t head_y = center_y + r->foot_offset;
    const int32_t probes[3] = {
        (int32_t)center_x - r->half_width + 1,
        (int32_t)center_x,
        (int32_t)center_x + r->half_width - 1
    };
    Dk1TerrainSample sample;
    size_t i;
    for (i = 0u; i < 3u; ++i) {
        if (dk1_rom_terrain_point_solid(&r->view, probes[i], head_y, &sample)) {
            if (contact != NULL) *contact = sample;
            return true;
        }
    }
    return false;
}

static void set_support(Dk1PlayerTerrainRuntime *r,
                        Dk1PlayerPreviewRuntime *p,
                        Dk1TerrainSample support,
                        bool was_airborne) {
    const int32_t stand = support.floor_y + r->foot_offset;
    const uint16_t previous_attributes = p->floor_attributes;
    p->motion.world_y = (int16_t)stand;
    p->motion.subpixel_y = 0u;
    p->motion.velocity_y = 0;
    p->floor_y = (int16_t)stand;
    p->floor_attributes = support.attributes;
    p->airborne = false;
    if (was_airborne) p->landings++;
    if (previous_attributes != support.attributes) r->material_changes++;
    r->support = support;
    r->last_attributes = support.attributes;
    r->resolved_contacts++;
}

bool dk1_player_terrain_init(Dk1PlayerTerrainRuntime *r,
                             const Dk1RomImage *rom,
                             Dk1LevelTerrainConfig config,
                             uint32_t maximum_x) {
    size_t columns;
    if (r == NULL || rom == NULL) return false;
    memset(r, 0, sizeof(*r));
    columns = (size_t)((maximum_x + 96u) / 32u + 2u);
    if (columns == 0u) columns = 1u;
    if (!dk1_terrain_shape_flags_from_rom(rom, r->shape_flags) ||
        !dk1_rom_terrain_view_init(&r->view, rom, config, columns, r->shape_flags))
        return false;
    r->column_count = columns;
    r->foot_offset = 16;
    r->half_width = 7;
    r->max_step_height = 14;
    r->ready = true;
    return true;
}

bool dk1_player_terrain_place(Dk1PlayerTerrainRuntime *r,
                              Dk1PlayerPreviewRuntime *p,
                              uint16_t viewport_height,
                              uint16_t *camera_y) {
    Dk1TerrainSample support;
    int32_t target, center, desired, max_camera;
    if (r == NULL || p == NULL || !r->ready) return false;
    target = (int32_t)p->motion.world_y - r->foot_offset;
    if (!best_support(r, p->motion.world_x, target, 512, 512, &support)) {
        r->misses++;
        return false;
    }
    center = support.floor_y + r->foot_offset;
    set_support(r, p, support, false);
    if (camera_y != NULL) {
        desired = 512 - center - (int32_t)viewport_height / 2;
        max_camera = 512 - (int32_t)viewport_height;
        if (max_camera < 0) max_camera = 0;
        if (desired < 0) desired = 0;
        if (desired > max_camera) desired = max_camera;
        *camera_y = (uint16_t)desired;
    }
    return true;
}

void dk1_player_terrain_resolve_motion(Dk1PlayerTerrainRuntime *r,
                                       Dk1PlayerPreviewRuntime *p,
                                       uint16_t old_world_x,
                                       int16_t old_world_y) {
    Dk1TerrainSample contact, support;
    int16_t delta_x;
    int32_t old_foot, new_foot;
    bool was_airborne, can_snap = false;
    if (r == NULL || p == NULL || !r->ready) return;

    r->blocked_left = false;
    r->blocked_right = false;
    r->hit_ceiling = false;

    delta_x = (int16_t)(uint16_t)(p->motion.world_x - old_world_x);
    if (delta_x != 0) {
        int32_t leading_x = (int32_t)p->motion.world_x +
                            (delta_x > 0 ? r->half_width : -r->half_width);
        if (body_side_solid(r, leading_x, p->motion.world_y, &contact)) {
            p->motion.world_x = old_world_x;
            p->motion.subpixel_x = 0u;
            p->motion.velocity_x = 0;
            p->motion.target_velocity_x = 0;
            r->wall_attributes = contact.attributes;
            r->blocked_right = delta_x > 0;
            r->blocked_left = delta_x < 0;
            r->wall_contacts++;
        }
    }

    if (p->motion.velocity_y > 0 && p->motion.world_y > old_world_y &&
        head_solid(r, p->motion.world_x, p->motion.world_y, &contact)) {
        p->motion.world_y = old_world_y;
        p->motion.subpixel_y = 0u;
        p->motion.velocity_y = 0;
        p->airborne = true;
        r->ceiling_attributes = contact.attributes;
        r->hit_ceiling = true;
        r->ceiling_contacts++;
        return;
    }

    old_foot = (int32_t)old_world_y - r->foot_offset;
    new_foot = (int32_t)p->motion.world_y - r->foot_offset;
    was_airborne = p->airborne;
    if (best_support(r, p->motion.world_x, new_foot, 40, 24, &support)) {
        if (p->motion.velocity_y <= 0) {
            if (was_airborne) {
                can_snap = support.floor_y >= new_foot - 6 &&
                           support.floor_y <= old_foot + 10;
            } else {
                int32_t rise = support.floor_y - new_foot;
                can_snap = abs((int)rise) <= r->max_step_height;
            }
        }
        if (can_snap) {
            set_support(r, p, support, was_airborne);
            return;
        }
    }
    p->airborne = true;
    r->misses++;
}

void dk1_player_terrain_resolve(Dk1PlayerTerrainRuntime *r,
                                Dk1PlayerPreviewRuntime *p,
                                int16_t old_world_y) {
    if (p == NULL) return;
    dk1_player_terrain_resolve_motion(r, p, p->motion.world_x, old_world_y);
}

void dk1_player_terrain_step(Dk1PlayerTerrainRuntime *r,
                             Dk1PlayerPreviewRuntime *p,
                             uint16_t held, uint16_t pressed) {
    uint16_t old_x;
    int16_t old_y;
    if (r == NULL || p == NULL || !r->ready) {
        dk1_player_preview_step(p, held, pressed);
        return;
    }
    old_x = p->motion.world_x;
    old_y = p->motion.world_y;
    dk1_player_preview_advance(p, held, pressed);
    dk1_player_terrain_resolve_motion(r, p, old_x, old_y);
}
