#ifndef DK1_DYNAMIC_BG1_RUNTIME_H
#define DK1_DYNAMIC_BG1_RUNTIME_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/dynamic_tile_stream.h"
#include "dk1/rgba_surface.h"
#include "dk1/scene_memory.h"
#include "dk1/scene_view.h"

#define DK1_DYNAMIC_BG1_MAP_COLUMNS 128u

typedef struct Dk1DynamicBg1Runtime {
    Dk1VramImage vram;
    Dk1PpuBgLayout layout;
    Dk1DynamicTileStreamStats last;
    size_t update_count;
    size_t map_columns_uploaded;
    size_t tile_columns_uploaded;
    size_t tile_words_uploaded;
    uint32_t generation;
    bool ready;
} Dk1DynamicBg1Runtime;

/* Copies the scene's initialized VRAM, then uploads the first visible map
 * envelope into the BG1 ring tilemap. Scenes without an enabled BG1 are valid
 * but leave runtime->ready false. */
bool dk1_dynamic_bg1_runtime_init(
    const Dk1RomImage *rom,
    const Dk1SceneMemory *scene,
    const Dk1DynamicStreamUpdate *initial_update,
    Dk1DynamicBg1Runtime *runtime);

/* Applies only newly entered 32-pixel map columns. Character graphics remain
 * those loaded from the legal ROM/cache; this runtime updates tilemap words. */
bool dk1_dynamic_bg1_runtime_update(
    const Dk1RomImage *rom,
    const Dk1SceneMemory *scene,
    const Dk1DynamicStreamUpdate *update,
    Dk1DynamicBg1Runtime *runtime);

bool dk1_dynamic_bg1_runtime_render(
    const Dk1SceneMemory *scene,
    Dk1SceneView view,
    const Dk1DynamicBg1Runtime *runtime,
    Dk1RgbaSurface destination);

uint64_t dk1_dynamic_bg1_runtime_signature(
    const Dk1DynamicBg1Runtime *runtime);

#endif
