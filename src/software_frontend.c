#include "dk1/software_frontend.h"
#include <stdlib.h>
#include <string.h>
#include "dk1/scene_signature.h"

static void update_marker(Dk1SoftwareFrontend *f) {
    Dk1OamSprite s;
    s.x = f->marker_x;
    s.y = (uint8_t)f->marker_y;
    s.tile_number = f->marker_tile;
    s.palette = f->marker_palette;
    s.priority = 3u;
    s.horizontal_flip = false;
    s.vertical_flip = false;
    s.large = false;
    s.width = 8u;
    s.height = 8u;
    dk1_oam_init(&f->oam);
    (void)dk1_oam_encode_sprite(&f->oam, f->obsel, 0u, &s);
    f->player_visual_ready = false;
}

static void update_pos(Dk1SoftwareFrontend *f) {
    int16_t dx = (int16_t)(uint16_t)(f->player_preview.motion.world_x -
                                     f->runtime.view.camera_x);
    int32_t y = (int32_t)f->player_vertical_origin -
                f->runtime.view.camera_y - f->player_preview.motion.world_y;
    f->marker_x = dx;
    if (y < -128) y = -128;
    if (y > 255) y = 255;
    f->marker_y = (int16_t)y;
    f->player_frame = f->player_preview.frame_id;
}

static bool update_visual(const Dk1RomImage *r, const Dk1SceneMemory *s,
                          Dk1SoftwareFrontend *f) {
    Dk1ObjectScreenTransform t;
    Dk1ObjectFrameOamStyle st;
    if (r == NULL) {
        update_marker(f);
        return true;
    }
    t.world_x = (int16_t)f->player_preview.motion.world_x;
    t.world_y = f->player_preview.motion.world_y;
    t.camera_x = (int16_t)f->runtime.view.camera_x;
    t.vertical_origin = (int16_t)f->player_vertical_origin;
    t.vertical_scroll = (int16_t)f->runtime.view.camera_y;
    t.vertical_offset = 0;
    t.object_flags = f->player_preview.facing_left ? 0x4000u : 0u;
    st.base_tile_number = f->player_tile_base;
    st.palette = f->marker_palette;
    st.priority = 3u;
    st.horizontal_flip = false;
    st.vertical_flip = false;
    f->player_visual_ready = dk1_player_visual_build(
        r, &s->vram, f->player_frame, t, st, f->obsel,
        &f->oam, &f->player_vram, &f->player_visual);
    if (!f->player_visual_ready) update_marker(f);
    return true;
}

static void release_additional_gnawty(Dk1SoftwareGnawtySlot *slot) {
    if (slot == NULL)
        return;
    free(slot->runtime);
    slot->runtime = NULL;
    slot->record_index = 0u;
    slot->ready = false;
}

static void refresh_gnawty_active_count(Dk1SoftwareFrontend *f) {
    size_t i;
    if (f == NULL)
        return;
    f->gnawty_active_count = 0u;
    if (f->gnawty_ready && f->gnawty.active)
        ++f->gnawty_active_count;
    for (i = 0u; i < DK1_SOFTWARE_FRONTEND_EXTRA_GNAWTIES; ++i) {
        const Dk1SoftwareGnawtySlot *slot = &f->additional_gnawties[i];
        if (slot->ready && slot->runtime != NULL && slot->runtime->active)
            ++f->gnawty_active_count;
    }
}

static bool update_gnawty_slot(Dk1GnawtyRuntime *runtime,
                               const Dk1LevelObjectStreamEntry *entry) {
    if (runtime == NULL || entry == NULL ||
        entry->slot >= DK1_OBJECT_SLOT_COUNT)
        return false;
    if (runtime->slot != entry->slot) {
        if (runtime->slot < DK1_OBJECT_SLOT_COUNT)
            runtime->scheduler.type_id[runtime->slot] = 0u;
        runtime->slot = entry->slot;
        runtime->scheduler.type_id[entry->slot] = DK1_OBJECT_TYPE_GNAWTY;
    }
    return true;
}

static const Dk1LevelObjectStreamEntry *active_gnawty_entry(
    const Dk1SoftwareFrontend *f, size_t record_index) {
    size_t i;
    if (f == NULL)
        return NULL;
    for (i = 0u; i < f->level_objects.catalog_count; ++i) {
        const Dk1LevelObjectStreamEntry *entry =
            &f->level_objects.entries[i];
        if (entry->active && entry->type_id == DK1_OBJECT_TYPE_GNAWTY &&
            entry->record_index == record_index)
            return entry;
    }
    return NULL;
}

static bool gnawty_record_bound(const Dk1SoftwareFrontend *f,
                                size_t record_index) {
    size_t i;
    if (f == NULL)
        return false;
    if (f->gnawty_ready && f->gnawty_record_index == record_index)
        return true;
    for (i = 0u; i < DK1_SOFTWARE_FRONTEND_EXTRA_GNAWTIES; ++i) {
        if (f->additional_gnawties[i].ready &&
            f->additional_gnawties[i].record_index == record_index)
            return true;
    }
    return false;
}

static bool spawn_additional_gnawty(
    Dk1SoftwareFrontend *f,
    Dk1SoftwareGnawtySlot *slot,
    const Dk1LevelObjectStreamEntry *entry) {
    if (f == NULL || slot == NULL || entry == NULL)
        return false;
    slot->runtime = (Dk1GnawtyRuntime *)calloc(1u, sizeof(*slot->runtime));
    if (slot->runtime == NULL)
        return false;
    if (!dk1_gnawty_spawn(slot->runtime, f->source_rom, entry->slot,
                          entry->record.world_x, entry->record.world_y,
                          false)) {
        release_additional_gnawty(slot);
        return false;
    }
    slot->record_index = entry->record_index;
    slot->ready = true;
    return true;
}

static bool init_common(const Dk1RomImage *r, const Dk1SceneMemory *s,
                        uint16_t w, uint16_t h, Dk1SoftwareFrontend *f) {
    uint8_t ob = 0;
    Dk1DynamicStreamUpdate u;
    uint16_t sx;
    int16_t sy;
    if (s == NULL || f == NULL || w == 0u || h == 0u) return false;
    memset(f, 0, sizeof(*f));
    f->source_rom = r;
    f->barrel_record_index = SIZE_MAX;
    f->runtime.view.width = w;
    f->runtime.view.height = h;
    f->marker_x = (int16_t)(w / 2u);
    f->marker_y = (int16_t)(h / 2u);
    f->player_frame = 0x0330u;
    f->player_tile_base = 0x0040u;
    f->player_vertical_origin = 224u;
    f->player_vram = s->vram;
    (void)dk1_ppu_preset_register(&s->ppu, 0x2101u, &ob);
    f->obsel = ob;
    if (!dk1_scene_view_clamp(s, &f->runtime.view)) return false;
    sx = (uint16_t)(f->runtime.view.camera_x + w / 2u);
    sy = (int16_t)(224 - f->runtime.view.camera_y - h / 2u);
    dk1_player_preview_init(&f->player_preview, sx, sy);
    dk1_player_live_init(&f->player_live);
    dk1_player_combat_init(&f->player_combat);
    if (r != NULL &&
        dk1_player_terrain_init(&f->player_terrain, r, s->terrain,
            (uint32_t)s->camera.maximum_x + w + 64u) &&
        dk1_player_terrain_place(&f->player_terrain, &f->player_preview,
                                 h, &f->runtime.view.camera_y)) {
        f->player_vertical_origin = 512u;
        f->player_terrain_ready = true;
    }
    update_pos(f);
    update_marker(f);
    return dk1_dynamic_stream_update(f->runtime.view, 128u, &f->stream, &u);
}

bool dk1_software_frontend_init(const Dk1SceneMemory *s, uint16_t w,
                                uint16_t h, Dk1SoftwareFrontend *f) {
    return init_common(NULL, s, w, h, f);
}

bool dk1_software_frontend_init_with_rom(const Dk1RomImage *r,
                                         const Dk1SceneMemory *s,
                                         uint16_t w, uint16_t h,
                                         Dk1SoftwareFrontend *f) {
    return r != NULL && init_common(r, s, w, h, f);
}

void dk1_software_frontend_dispose(Dk1SoftwareFrontend *f) {
    size_t i;
    if (f == NULL)
        return;
    dk1_level_barrel_pool_dispose(&f->streamed_barrels);
    for (i = 0u; i < DK1_SOFTWARE_FRONTEND_EXTRA_GNAWTIES; ++i)
        release_additional_gnawty(&f->additional_gnawties[i]);
    f->barrel.live.active = false;
    f->barrel_ready = false;
    f->gnawty.active = false;
    f->gnawty_ready = false;
    f->gnawty_active_count = 0u;
    f->gnawty_capacity_overflow_count = 0u;
}

bool dk1_software_frontend_spawn_barrel(Dk1SoftwareFrontend *f,
                                        uint16_t type_id,
                                        uint16_t world_x,
                                        int16_t world_y) {
    if (f == NULL || f->source_rom == NULL || !f->player_terrain_ready)
        return false;
    f->barrel_ready = dk1_barrel_scene_spawn(
        &f->barrel, f->source_rom, 1u, type_id, world_x, world_y);
    return f->barrel_ready;
}

bool dk1_software_frontend_sync_barrels(Dk1SoftwareFrontend *f) {
    if (f == NULL)
        return false;
    if (f->source_rom == NULL || !f->level_objects_ready) {
        dk1_level_barrel_pool_dispose(&f->streamed_barrels);
        return true;
    }
    return dk1_level_barrel_pool_sync(
        &f->streamed_barrels, f->source_rom, &f->level_objects,
        f->defeated_level_records, f->barrel_record_index);
}

bool dk1_software_frontend_sync_gnawty(Dk1SoftwareFrontend *f) {
    size_t i;
    if (f == NULL)
        return false;

    f->gnawty_active_count = 0u;
    f->gnawty_capacity_overflow_count = 0u;
    if (f->source_rom == NULL || !f->level_objects_ready) {
        size_t j;
        for (j = 0u; j < DK1_SOFTWARE_FRONTEND_EXTRA_GNAWTIES; ++j)
            release_additional_gnawty(&f->additional_gnawties[j]);
        f->gnawty.active = false;
        f->gnawty_ready = false;
        return true;
    }

    if (f->gnawty_ready) {
        const Dk1LevelObjectStreamEntry *entry =
            active_gnawty_entry(f, f->gnawty_record_index);
        if (entry == NULL ||
            f->gnawty_record_index >= DK1_LEVEL_OBJECT_STREAM_MAX_ENTRIES ||
            f->defeated_level_records[f->gnawty_record_index]) {
            if (f->gnawty.slot < DK1_OBJECT_SLOT_COUNT)
                f->gnawty.scheduler.type_id[f->gnawty.slot] = 0u;
            f->gnawty.active = false;
            f->gnawty_ready = false;
        } else if (!update_gnawty_slot(&f->gnawty, entry)) {
            return false;
        }
    }

    for (i = 0u; i < DK1_SOFTWARE_FRONTEND_EXTRA_GNAWTIES; ++i) {
        Dk1SoftwareGnawtySlot *slot = &f->additional_gnawties[i];
        const Dk1LevelObjectStreamEntry *entry;
        if (!slot->ready)
            continue;
        entry = active_gnawty_entry(f, slot->record_index);
        if (entry == NULL ||
            slot->record_index >= DK1_LEVEL_OBJECT_STREAM_MAX_ENTRIES ||
            f->defeated_level_records[slot->record_index]) {
            release_additional_gnawty(slot);
        } else if (!update_gnawty_slot(slot->runtime, entry)) {
            return false;
        }
    }

    for (i = 0u; i < f->level_objects.catalog_count; ++i) {
        const Dk1LevelObjectStreamEntry *entry =
            &f->level_objects.entries[i];
        size_t j;
        if (!entry->active || entry->type_id != DK1_OBJECT_TYPE_GNAWTY ||
            entry->record_index >= DK1_LEVEL_OBJECT_STREAM_MAX_ENTRIES ||
            f->defeated_level_records[entry->record_index] ||
            gnawty_record_bound(f, entry->record_index))
            continue;

        if (!f->gnawty_ready) {
            if (!dk1_gnawty_spawn(&f->gnawty, f->source_rom, entry->slot,
                                   entry->record.world_x,
                                   entry->record.world_y, false))
                return false;
            f->gnawty_record_index = entry->record_index;
            f->gnawty_ready = true;
            continue;
        }

        for (j = 0u; j < DK1_SOFTWARE_FRONTEND_EXTRA_GNAWTIES; ++j) {
            if (!f->additional_gnawties[j].ready)
                break;
        }
        if (j == DK1_SOFTWARE_FRONTEND_EXTRA_GNAWTIES) {
            ++f->gnawty_capacity_overflow_count;
            continue;
        }
        if (!spawn_additional_gnawty(f, &f->additional_gnawties[j], entry))
            return false;
    }

    refresh_gnawty_active_count(f);
    return true;
}

static bool step_gnawty(Dk1SoftwareFrontend *f,
                        Dk1GnawtyRuntime *runtime,
                        size_t record_index,
                        Dk1GnawtyStepResult *result) {
    if (!dk1_gnawty_step(runtime, f->source_rom,
            f->player_terrain_ready ? &f->player_terrain : NULL,
            &f->player_preview, result))
        return false;
    if (result->stomped &&
        record_index < DK1_LEVEL_OBJECT_STREAM_MAX_ENTRIES)
        f->defeated_level_records[record_index] = true;
    if (result->player_hurt)
        (void)dk1_player_combat_apply_enemy_hit(
            &f->player_combat, &f->player_preview,
            runtime->motion.world_x);
    return true;
}

bool dk1_software_frontend_step(const Dk1SceneMemory *s, uint16_t held,
                                Dk1SoftwareFrontend *f) {
    Dk1DynamicStreamUpdate u;
    Dk1BarrelLiveResult barrel_result;
    Dk1LevelBarrelPoolStepResult streamed_barrel_result;
    Dk1GnawtyStepResult gnawty_result;
    uint16_t embedded_barrel_pressed;
    uint16_t streamed_barrel_pressed;
    uint32_t maxx;
    size_t i;
    bool streamed_barrel_held;
    if (s == NULL || f == NULL) return false;
    memset(&barrel_result, 0, sizeof(barrel_result));
    memset(&streamed_barrel_result, 0, sizeof(streamed_barrel_result));
    dk1_host_input_update(&f->input, held);
    dk1_player_combat_step(&f->player_combat);
    if (!dk1_scene_runtime_step(s, held, &f->runtime)) return false;
    if (!dk1_dynamic_stream_update(f->runtime.view, 128u, &f->stream, &u))
        return false;
    if (f->level_objects_ready &&
        !dk1_level_object_stream_update(
            &f->level_objects, f->runtime.view.camera_x,
            f->runtime.view.width, 128u, 128u))
        return false;
    if (!dk1_software_frontend_sync_gnawty(f) ||
        !dk1_software_frontend_sync_barrels(f))
        return false;
    dk1_player_live_step(&f->player_live, &f->player_preview,
        f->player_terrain_ready ? &f->player_terrain : NULL,
        held, f->input.pressed);

    if (f->gnawty_ready && f->gnawty.active) {
        if (!step_gnawty(f, &f->gnawty, f->gnawty_record_index,
                         &gnawty_result))
            return false;
        if (!f->gnawty.active)
            f->gnawty_ready = false;
    }
    for (i = 0u; i < DK1_SOFTWARE_FRONTEND_EXTRA_GNAWTIES; ++i) {
        Dk1SoftwareGnawtySlot *slot = &f->additional_gnawties[i];
        if (!slot->ready || slot->runtime == NULL || !slot->runtime->active)
            continue;
        if (!step_gnawty(f, slot->runtime, slot->record_index,
                         &gnawty_result))
            return false;
        if (!slot->runtime->active)
            release_additional_gnawty(slot);
    }
    refresh_gnawty_active_count(f);

    streamed_barrel_held =
        dk1_level_barrel_pool_has_held(&f->streamed_barrels);
    embedded_barrel_pressed = streamed_barrel_held ? 0u : f->input.pressed;
    if (f->barrel_ready && f->barrel.live.active) {
        if (!dk1_barrel_scene_step(&f->barrel, f->source_rom,
                f->player_terrain_ready ? &f->player_terrain : NULL,
                &f->player_preview, embedded_barrel_pressed, &barrel_result))
            return false;
        if (!f->barrel.live.active) {
            if (f->barrel_record_index <
                DK1_LEVEL_OBJECT_STREAM_MAX_ENTRIES)
                f->defeated_level_records[f->barrel_record_index] = true;
            f->barrel_ready = false;
        }
    }

    if (streamed_barrel_held) {
        streamed_barrel_pressed = f->input.pressed;
    } else if ((f->barrel_ready && f->barrel.live.active &&
                f->barrel.live.phase == DK1_BARREL_LIVE_HELD) ||
               barrel_result.picked_up || barrel_result.thrown) {
        streamed_barrel_pressed = 0u;
    } else {
        streamed_barrel_pressed = f->input.pressed;
    }
    if (f->source_rom != NULL && f->level_objects_ready &&
        !dk1_level_barrel_pool_step(
            &f->streamed_barrels, f->source_rom,
            f->player_terrain_ready ? &f->player_terrain : NULL,
            &f->player_preview, streamed_barrel_pressed,
            f->defeated_level_records, &streamed_barrel_result))
        return false;

    maxx = (uint32_t)s->camera.maximum_x + f->runtime.view.width + 32u;
    if (f->player_preview.motion.world_x > maxx)
        f->player_preview.motion.world_x = (uint16_t)maxx;
    update_pos(f);
    f->player_visual_ready = false;
    return true;
}

static bool render_gnawty(const Dk1RomImage *r,
                          const Dk1SceneMemory *s,
                          Dk1SoftwareFrontend *f,
                          Dk1GnawtyRuntime *runtime,
                          Dk1RgbaSurface d) {
    if (!dk1_gnawty_build_visual(
            runtime, r, &s->vram,
            f->runtime.view.camera_x, f->runtime.view.camera_y,
            f->player_vertical_origin, f->obsel))
        return false;
    return dk1_oam_render(&runtime->oam, f->obsel, &runtime->vram,
                          &s->cgram, 0u, 3u, d);
}

bool dk1_software_frontend_render(const Dk1RomImage *r,
                                  const Dk1SceneMemory *s,
                                  Dk1SoftwareFrontend *f,
                                  Dk1RgbaSurface d) {
    Dk1Rgba8 *px = NULL;
    Dk1RgbaSurface st;
    const Dk1VramImage *sv;
    uint8_t disp = 15u;
    bool streamed = false;
    size_t i, n;
    if (s == NULL || f == NULL || d.pixels == NULL ||
        d.width != f->runtime.view.width || d.height != f->runtime.view.height)
        return false;
    if (!dk1_ppu_compose_mode1(s, f->runtime.view.camera_x / 2u,
                               f->runtime.view.camera_y / 2u, 0u,
                               f->runtime.view.camera_y, d))
        return false;
    if (r != NULL) {
        px = calloc(d.width * d.height, sizeof(*px));
        if (px != NULL) {
            st = (Dk1RgbaSurface){px, d.width, d.height, d.width};
            streamed = dk1_scene_render_bg1(r, s, f->runtime.view, st);
            if (streamed)
                memcpy(d.pixels, px, d.width * d.height * sizeof(*px));
            free(px);
        }
    }
    if (f->barrel_ready && f->barrel.live.active) {
        if (!dk1_barrel_scene_build_visual(
                &f->barrel, r, &s->vram,
                f->runtime.view.camera_x, f->runtime.view.camera_y,
                f->player_vertical_origin, f->obsel))
            return false;
        if (!dk1_oam_render(&f->barrel.oam, f->obsel, &f->barrel.vram,
                            &s->cgram, 0u, 3u, d))
            return false;
    }
    if (r != NULL &&
        !dk1_level_barrel_pool_render(
            &f->streamed_barrels, r, s,
            f->runtime.view.camera_x, f->runtime.view.camera_y,
            f->player_vertical_origin, f->obsel, d))
        return false;
    if (f->gnawty_ready && f->gnawty.active &&
        !render_gnawty(r, s, f, &f->gnawty, d))
        return false;
    for (i = 0u; i < DK1_SOFTWARE_FRONTEND_EXTRA_GNAWTIES; ++i) {
        Dk1SoftwareGnawtySlot *slot = &f->additional_gnawties[i];
        if (slot->ready && slot->runtime != NULL && slot->runtime->active &&
            !render_gnawty(r, s, f, slot->runtime, d))
            return false;
    }
    if (dk1_player_combat_should_render(&f->player_combat)) {
        if (!update_visual(r, s, f)) return false;
        sv = f->player_visual_ready ? &f->player_vram : &s->vram;
        if (!dk1_oam_render(&f->oam, f->obsel, sv, &s->cgram, 0u, 3u, d))
            return false;
    } else {
        f->player_visual_ready = false;
    }
    if (dk1_ppu_preset_register(&s->ppu, 0x2100u, &disp)) disp &= 15u;
    n = d.width * d.height;
    if (disp < 15u) {
        for (i = 0u; i < n; ++i) {
            uint16_t rr = d.pixels[i].r;
            uint16_t g = d.pixels[i].g;
            uint16_t b = d.pixels[i].b;
            d.pixels[i].r = (uint8_t)((rr * disp + 7u) / 15u);
            d.pixels[i].g = (uint8_t)((g * disp + 7u) / 15u);
            d.pixels[i].b = (uint8_t)((b * disp + 7u) / 15u);
        }
    }
    return true;
}

uint64_t dk1_software_frontend_signature(const Dk1SoftwareFrontend *f) {
    uint64_t h = 0;
    uint64_t barrel_pool_signature;
    size_t i;
    if (f == NULL) return 0u;
    h = dk1_fnv1a64(&f->runtime, sizeof(f->runtime), h);
    h = dk1_fnv1a64(&f->input, sizeof(f->input), h);
    h = dk1_fnv1a64(&f->stream, sizeof(f->stream), h);
    h = dk1_fnv1a64(&f->level_objects_ready,
                    sizeof(f->level_objects_ready), h);
    if (f->level_objects_ready)
        h = dk1_fnv1a64(&f->level_objects, sizeof(f->level_objects), h);
    h = dk1_fnv1a64(&f->defeated_level_records,
                    sizeof(f->defeated_level_records), h);
    h = dk1_fnv1a64(&f->gnawty_ready, sizeof(f->gnawty_ready), h);
    if (f->gnawty_ready) {
        h = dk1_fnv1a64(&f->gnawty_record_index,
                        sizeof(f->gnawty_record_index), h);
        h = dk1_fnv1a64(&f->gnawty, sizeof(f->gnawty), h);
    }
    h = dk1_fnv1a64(&f->gnawty_active_count,
                    sizeof(f->gnawty_active_count), h);
    h = dk1_fnv1a64(&f->gnawty_capacity_overflow_count,
                    sizeof(f->gnawty_capacity_overflow_count), h);
    for (i = 0u; i < DK1_SOFTWARE_FRONTEND_EXTRA_GNAWTIES; ++i) {
        const Dk1SoftwareGnawtySlot *slot = &f->additional_gnawties[i];
        h = dk1_fnv1a64(&slot->ready, sizeof(slot->ready), h);
        if (slot->ready && slot->runtime != NULL) {
            h = dk1_fnv1a64(&slot->record_index,
                            sizeof(slot->record_index), h);
            h = dk1_fnv1a64(slot->runtime, sizeof(*slot->runtime), h);
        }
    }
    h = dk1_fnv1a64(&f->player_preview, sizeof(f->player_preview), h);
    h = dk1_fnv1a64(&f->player_live, sizeof(f->player_live), h);
    h = dk1_fnv1a64(&f->player_combat, sizeof(f->player_combat), h);
    h = dk1_fnv1a64(&f->player_terrain.last_attributes,
                    sizeof(f->player_terrain.last_attributes), h);
    h = dk1_fnv1a64(&f->marker_x, sizeof(f->marker_x), h);
    h = dk1_fnv1a64(&f->marker_y, sizeof(f->marker_y), h);
    h = dk1_fnv1a64(&f->player_frame, sizeof(f->player_frame), h);
    h = dk1_fnv1a64(&f->player_visual, sizeof(f->player_visual), h);
    h = dk1_fnv1a64(&f->barrel_record_index,
                    sizeof(f->barrel_record_index), h);
    h = dk1_fnv1a64(&f->barrel_ready, sizeof(f->barrel_ready), h);
    if (f->barrel_ready) {
        h = dk1_fnv1a64(&f->barrel.live, sizeof(f->barrel.live), h);
        h = dk1_fnv1a64(&f->barrel.selected_animation,
                        sizeof(f->barrel.selected_animation), h);
        h = dk1_fnv1a64(&f->barrel.visual, sizeof(f->barrel.visual), h);
    }
    barrel_pool_signature =
        dk1_level_barrel_pool_signature(&f->streamed_barrels);
    h = dk1_fnv1a64(&barrel_pool_signature,
                    sizeof(barrel_pool_signature), h);
    return dk1_fnv1a64(&f->oam, sizeof(f->oam), h);
}
