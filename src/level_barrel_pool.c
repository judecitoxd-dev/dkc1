#include "dk1/level_barrel_pool.h"
#include <stdlib.h>
#include <string.h>
#include "dk1/scene_signature.h"

#define DK1_LEVEL_BARREL_RUNTIME_MARGIN 256

static void release_slot(Dk1LevelBarrelSlot *slot) {
    if (slot == NULL)
        return;
    free(slot->runtime);
    slot->runtime = NULL;
    slot->record_index = 0u;
    slot->ready = false;
}

bool dk1_level_barrel_type_supported(uint16_t type_id) {
    return type_id == DK1_OBJECT_TYPE_STEEL_KEG ||
           type_id == DK1_OBJECT_TYPE_BARREL ||
           type_id == DK1_OBJECT_TYPE_ROPE_BARREL ||
           type_id == DK1_OBJECT_TYPE_DK_BARREL ||
           type_id == DK1_OBJECT_TYPE_TNT_BARREL;
}

static const Dk1LevelObjectStreamEntry *active_entry(
    const Dk1LevelObjectStream *stream,
    size_t record_index) {
    size_t i;
    if (stream == NULL)
        return NULL;
    for (i = 0u; i < stream->catalog_count; ++i) {
        const Dk1LevelObjectStreamEntry *entry = &stream->entries[i];
        if (entry->active && entry->record_index == record_index &&
            dk1_level_barrel_type_supported(entry->type_id))
            return entry;
    }
    return NULL;
}

static bool record_bound(const Dk1LevelBarrelPool *pool,
                         size_t record_index) {
    size_t i;
    if (pool == NULL)
        return false;
    for (i = 0u; i < DK1_LEVEL_BARREL_POOL_CAPACITY; ++i) {
        if (pool->slots[i].ready &&
            pool->slots[i].record_index == record_index)
            return true;
    }
    return false;
}

static bool runtime_in_envelope(const Dk1LevelObjectStream *stream,
                                const Dk1BarrelSceneRuntime *runtime) {
    int32_t left;
    int32_t right;
    int32_t x;
    if (stream == NULL || runtime == NULL)
        return false;
    left = (int32_t)stream->camera_x - DK1_LEVEL_BARREL_RUNTIME_MARGIN;
    right = (int32_t)stream->camera_x + stream->viewport_width +
            DK1_LEVEL_BARREL_RUNTIME_MARGIN;
    x = runtime->live.motion.world_x;
    return x >= left && x <= right;
}

static void refresh_active_count(Dk1LevelBarrelPool *pool) {
    size_t i;
    if (pool == NULL)
        return;
    pool->active_count = 0u;
    for (i = 0u; i < DK1_LEVEL_BARREL_POOL_CAPACITY; ++i) {
        const Dk1LevelBarrelSlot *slot = &pool->slots[i];
        if (slot->ready && slot->runtime != NULL &&
            slot->runtime->live.active)
            ++pool->active_count;
    }
}

static bool spawn_slot(Dk1LevelBarrelSlot *slot,
                       const Dk1RomImage *rom,
                       const Dk1LevelObjectStreamEntry *entry) {
    if (slot == NULL || rom == NULL || entry == NULL)
        return false;
    slot->runtime = (Dk1BarrelSceneRuntime *)calloc(1u,
                                                    sizeof(*slot->runtime));
    if (slot->runtime == NULL)
        return false;
    if (!dk1_barrel_scene_spawn(slot->runtime, rom, entry->slot,
                                entry->type_id, entry->record.world_x,
                                entry->record.world_y)) {
        release_slot(slot);
        return false;
    }
    slot->record_index = entry->record_index;
    slot->ready = true;
    return true;
}

bool dk1_level_barrel_pool_sync(
    Dk1LevelBarrelPool *pool,
    const Dk1RomImage *rom,
    const Dk1LevelObjectStream *stream,
    const bool defeated_records[DK1_LEVEL_OBJECT_STREAM_MAX_ENTRIES],
    size_t skip_record_index) {
    size_t i;
    if (pool == NULL || rom == NULL || stream == NULL ||
        defeated_records == NULL)
        return false;

    pool->entered_count = 0u;
    pool->retained_count = 0u;
    pool->exited_count = 0u;
    pool->overflow_count = 0u;

    for (i = 0u; i < DK1_LEVEL_BARREL_POOL_CAPACITY; ++i) {
        Dk1LevelBarrelSlot *slot = &pool->slots[i];
        const Dk1LevelObjectStreamEntry *entry;
        if (!slot->ready)
            continue;
        if (slot->runtime == NULL || !slot->runtime->live.active ||
            slot->record_index >= DK1_LEVEL_OBJECT_STREAM_MAX_ENTRIES ||
            defeated_records[slot->record_index]) {
            release_slot(slot);
            ++pool->exited_count;
            continue;
        }
        entry = active_entry(stream, slot->record_index);
        if (entry != NULL) {
            slot->runtime->live.slot = entry->slot;
            ++pool->retained_count;
            continue;
        }
        if (slot->runtime->live.phase == DK1_BARREL_LIVE_IDLE ||
            !runtime_in_envelope(stream, slot->runtime)) {
            release_slot(slot);
            ++pool->exited_count;
        } else {
            ++pool->retained_count;
        }
    }

    for (i = 0u; i < stream->catalog_count; ++i) {
        const Dk1LevelObjectStreamEntry *entry = &stream->entries[i];
        size_t j;
        if (!entry->active ||
            !dk1_level_barrel_type_supported(entry->type_id) ||
            entry->record_index == skip_record_index ||
            entry->record_index >= DK1_LEVEL_OBJECT_STREAM_MAX_ENTRIES ||
            defeated_records[entry->record_index] ||
            record_bound(pool, entry->record_index))
            continue;
        for (j = 0u; j < DK1_LEVEL_BARREL_POOL_CAPACITY; ++j) {
            if (!pool->slots[j].ready)
                break;
        }
        if (j == DK1_LEVEL_BARREL_POOL_CAPACITY) {
            ++pool->overflow_count;
            continue;
        }
        if (!spawn_slot(&pool->slots[j], rom, entry))
            return false;
        ++pool->entered_count;
    }

    refresh_active_count(pool);
    return true;
}

bool dk1_level_barrel_pool_has_held(const Dk1LevelBarrelPool *pool) {
    size_t i;
    if (pool == NULL)
        return false;
    for (i = 0u; i < DK1_LEVEL_BARREL_POOL_CAPACITY; ++i) {
        const Dk1LevelBarrelSlot *slot = &pool->slots[i];
        if (slot->ready && slot->runtime != NULL &&
            slot->runtime->live.active &&
            slot->runtime->live.phase == DK1_BARREL_LIVE_HELD)
            return true;
    }
    return false;
}

static bool step_slot(
    Dk1LevelBarrelPool *pool,
    Dk1LevelBarrelSlot *slot,
    const Dk1RomImage *rom,
    Dk1PlayerTerrainRuntime *terrain,
    const Dk1PlayerPreviewRuntime *player,
    uint16_t pressed,
    bool defeated_records[DK1_LEVEL_OBJECT_STREAM_MAX_ENTRIES],
    Dk1LevelBarrelPoolStepResult *aggregate) {
    Dk1BarrelLiveResult local;
    if (pool == NULL || slot == NULL || slot->runtime == NULL ||
        aggregate == NULL)
        return false;
    if (!dk1_barrel_scene_step(slot->runtime, rom, terrain, player,
                               pressed, &local))
        return false;
    ++aggregate->stepped;
    if (local.picked_up) {
        aggregate->picked_up = true;
        aggregate->action_consumed = true;
        ++pool->pickups;
    }
    if (local.thrown) {
        aggregate->thrown = true;
        aggregate->action_consumed = true;
        ++pool->throws;
    }
    aggregate->exploded = aggregate->exploded || local.exploded;
    aggregate->released_kong =
        aggregate->released_kong || local.released_kong;
    if (local.destroyed || !slot->runtime->live.active) {
        if (slot->record_index < DK1_LEVEL_OBJECT_STREAM_MAX_ENTRIES)
            defeated_records[slot->record_index] = true;
        release_slot(slot);
        ++aggregate->destroyed;
        ++pool->destroyed;
    }
    return true;
}

bool dk1_level_barrel_pool_step(
    Dk1LevelBarrelPool *pool,
    const Dk1RomImage *rom,
    Dk1PlayerTerrainRuntime *terrain,
    const Dk1PlayerPreviewRuntime *player,
    uint16_t pressed,
    bool defeated_records[DK1_LEVEL_OBJECT_STREAM_MAX_ENTRIES],
    Dk1LevelBarrelPoolStepResult *result) {
    size_t i;
    size_t held_index = DK1_LEVEL_BARREL_POOL_CAPACITY;
    uint16_t remaining_pressed = pressed;
    if (pool == NULL || rom == NULL || player == NULL ||
        defeated_records == NULL || result == NULL)
        return false;
    memset(result, 0, sizeof(*result));

    for (i = 0u; i < DK1_LEVEL_BARREL_POOL_CAPACITY; ++i) {
        const Dk1LevelBarrelSlot *slot = &pool->slots[i];
        if (slot->ready && slot->runtime != NULL &&
            slot->runtime->live.active &&
            slot->runtime->live.phase == DK1_BARREL_LIVE_HELD) {
            held_index = i;
            break;
        }
    }

    if (held_index < DK1_LEVEL_BARREL_POOL_CAPACITY) {
        if (!step_slot(pool, &pool->slots[held_index], rom, terrain, player,
                       pressed, defeated_records, result))
            return false;
        remaining_pressed = 0u;
    }

    for (i = 0u; i < DK1_LEVEL_BARREL_POOL_CAPACITY; ++i) {
        Dk1LevelBarrelSlot *slot = &pool->slots[i];
        bool consumed_before;
        if (i == held_index || !slot->ready || slot->runtime == NULL ||
            !slot->runtime->live.active)
            continue;
        consumed_before = result->action_consumed;
        if (!step_slot(pool, slot, rom, terrain, player,
                       remaining_pressed, defeated_records, result))
            return false;
        if (!consumed_before && result->action_consumed)
            remaining_pressed = 0u;
    }

    refresh_active_count(pool);
    result->active = pool->active_count;
    return true;
}

bool dk1_level_barrel_pool_render(
    Dk1LevelBarrelPool *pool,
    const Dk1RomImage *rom,
    const Dk1SceneMemory *scene,
    uint16_t camera_x,
    uint16_t camera_y,
    uint16_t vertical_origin,
    uint8_t obsel,
    Dk1RgbaSurface destination) {
    size_t i;
    if (pool == NULL || rom == NULL || scene == NULL)
        return false;
    for (i = 0u; i < DK1_LEVEL_BARREL_POOL_CAPACITY; ++i) {
        Dk1LevelBarrelSlot *slot = &pool->slots[i];
        if (!slot->ready || slot->runtime == NULL ||
            !slot->runtime->live.active)
            continue;
        if (!dk1_barrel_scene_build_visual(slot->runtime, rom, &scene->vram,
                                            camera_x, camera_y,
                                            vertical_origin, obsel) ||
            !dk1_oam_render(&slot->runtime->oam, obsel,
                            &slot->runtime->vram, &scene->cgram,
                            0u, 3u, destination))
            return false;
    }
    return true;
}

void dk1_level_barrel_pool_dispose(Dk1LevelBarrelPool *pool) {
    size_t i;
    if (pool == NULL)
        return;
    for (i = 0u; i < DK1_LEVEL_BARREL_POOL_CAPACITY; ++i)
        release_slot(&pool->slots[i]);
    pool->active_count = 0u;
    pool->entered_count = 0u;
    pool->retained_count = 0u;
    pool->exited_count = 0u;
    pool->overflow_count = 0u;
}

uint64_t dk1_level_barrel_pool_signature(const Dk1LevelBarrelPool *pool) {
    uint64_t hash = 0u;
    size_t i;
    if (pool == NULL)
        return 0u;
    hash = dk1_fnv1a64(&pool->active_count, sizeof(pool->active_count), hash);
    hash = dk1_fnv1a64(&pool->entered_count, sizeof(pool->entered_count), hash);
    hash = dk1_fnv1a64(&pool->retained_count, sizeof(pool->retained_count), hash);
    hash = dk1_fnv1a64(&pool->exited_count, sizeof(pool->exited_count), hash);
    hash = dk1_fnv1a64(&pool->overflow_count, sizeof(pool->overflow_count), hash);
    hash = dk1_fnv1a64(&pool->pickups, sizeof(pool->pickups), hash);
    hash = dk1_fnv1a64(&pool->throws, sizeof(pool->throws), hash);
    hash = dk1_fnv1a64(&pool->destroyed, sizeof(pool->destroyed), hash);
    for (i = 0u; i < DK1_LEVEL_BARREL_POOL_CAPACITY; ++i) {
        const Dk1LevelBarrelSlot *slot = &pool->slots[i];
        hash = dk1_fnv1a64(&slot->ready, sizeof(slot->ready), hash);
        if (slot->ready && slot->runtime != NULL) {
            hash = dk1_fnv1a64(&slot->record_index,
                               sizeof(slot->record_index), hash);
            hash = dk1_fnv1a64(slot->runtime,
                               sizeof(*slot->runtime), hash);
        }
    }
    return hash;
}
