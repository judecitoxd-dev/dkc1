#ifndef DK1_LEVEL_BARREL_POOL_H
#define DK1_LEVEL_BARREL_POOL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/barrel_scene_runtime.h"
#include "dk1/level_object_stream.h"
#include "dk1/rgba_surface.h"
#include "dk1/scene_memory.h"

#define DK1_LEVEL_BARREL_POOL_CAPACITY DK1_PRIMARY_OBJECT_SLOT_COUNT

typedef struct Dk1LevelBarrelSlot {
    Dk1BarrelSceneRuntime *runtime;
    size_t record_index;
    bool ready;
} Dk1LevelBarrelSlot;

typedef struct Dk1LevelBarrelPool {
    Dk1LevelBarrelSlot slots[DK1_LEVEL_BARREL_POOL_CAPACITY];
    size_t active_count;
    size_t entered_count;
    size_t retained_count;
    size_t exited_count;
    size_t overflow_count;
    size_t pickups;
    size_t throws;
    size_t destroyed;
} Dk1LevelBarrelPool;

typedef struct Dk1LevelBarrelPoolStepResult {
    size_t stepped;
    size_t active;
    size_t destroyed;
    bool picked_up;
    bool thrown;
    bool exploded;
    bool released_kong;
    bool action_consumed;
} Dk1LevelBarrelPoolStepResult;

bool dk1_level_barrel_type_supported(uint16_t type_id);

/* Synchronizes idle source barrels to the camera stream. A carried, thrown or
 * rolling barrel remains alive after its source record exits until it is
 * destroyed or moves outside a wider runtime envelope. skip_record_index is
 * used by the legacy embedded Barrel while callers migrate to this pool. */
bool dk1_level_barrel_pool_sync(
    Dk1LevelBarrelPool *pool,
    const Dk1RomImage *rom,
    const Dk1LevelObjectStream *stream,
    const bool defeated_records[DK1_LEVEL_OBJECT_STREAM_MAX_ENTRIES],
    size_t skip_record_index);

bool dk1_level_barrel_pool_has_held(const Dk1LevelBarrelPool *pool);

bool dk1_level_barrel_pool_step(
    Dk1LevelBarrelPool *pool,
    const Dk1RomImage *rom,
    Dk1PlayerTerrainRuntime *terrain,
    const Dk1PlayerPreviewRuntime *player,
    uint16_t pressed,
    bool defeated_records[DK1_LEVEL_OBJECT_STREAM_MAX_ENTRIES],
    Dk1LevelBarrelPoolStepResult *result);

bool dk1_level_barrel_pool_render(
    Dk1LevelBarrelPool *pool,
    const Dk1RomImage *rom,
    const Dk1SceneMemory *scene,
    uint16_t camera_x,
    uint16_t camera_y,
    uint16_t vertical_origin,
    uint8_t obsel,
    Dk1RgbaSurface destination);

void dk1_level_barrel_pool_dispose(Dk1LevelBarrelPool *pool);
uint64_t dk1_level_barrel_pool_signature(const Dk1LevelBarrelPool *pool);

#endif
