#include "dk1/dynamic_bg1_runtime.h"
#include <string.h>
#include "dk1/ppu_bg_renderer.h"
#include "dk1/scene_signature.h"

static void accumulate(Dk1DynamicBg1Runtime *runtime) {
    if (runtime == NULL)
        return;
    if (runtime->last.map_columns != 0u)
        ++runtime->update_count;
    runtime->map_columns_uploaded += runtime->last.map_columns;
    runtime->tile_columns_uploaded += runtime->last.tile_columns;
    runtime->tile_words_uploaded += runtime->last.tile_words;
    if (runtime->last.map_columns != 0u)
        ++runtime->generation;
}

bool dk1_dynamic_bg1_runtime_init(
    const Dk1RomImage *rom,
    const Dk1SceneMemory *scene,
    const Dk1DynamicStreamUpdate *initial_update,
    Dk1DynamicBg1Runtime *runtime) {
    if (rom == NULL || scene == NULL || initial_update == NULL ||
        runtime == NULL)
        return false;
    memset(runtime, 0, sizeof(*runtime));
    runtime->vram = scene->vram;
    if (!dk1_ppu_bg_layout(&scene->ppu, 1u, &runtime->layout) ||
        !runtime->layout.main_enabled)
        return true;
    if (!dk1_dynamic_tile_stream_apply(
            rom, &scene->terrain, &runtime->layout, initial_update,
            &runtime->vram, &runtime->last)) {
        memset(&runtime->last, 0, sizeof(runtime->last));
        return true;
    }
    runtime->ready = true;
    accumulate(runtime);
    return true;
}

bool dk1_dynamic_bg1_runtime_update(
    const Dk1RomImage *rom,
    const Dk1SceneMemory *scene,
    const Dk1DynamicStreamUpdate *update,
    Dk1DynamicBg1Runtime *runtime) {
    if (rom == NULL || scene == NULL || update == NULL || runtime == NULL)
        return false;
    memset(&runtime->last, 0, sizeof(runtime->last));
    if (!runtime->ready || !update->changed)
        return true;
    if (!dk1_dynamic_tile_stream_apply(
            rom, &scene->terrain, &runtime->layout, update,
            &runtime->vram, &runtime->last)) {
        runtime->ready = false;
        memset(&runtime->last, 0, sizeof(runtime->last));
        return true;
    }
    accumulate(runtime);
    return true;
}

bool dk1_dynamic_bg1_runtime_render(
    const Dk1SceneMemory *scene,
    Dk1SceneView view,
    const Dk1DynamicBg1Runtime *runtime,
    Dk1RgbaSurface destination) {
    if (scene == NULL || runtime == NULL || !runtime->ready ||
        destination.pixels == NULL || destination.width < view.width ||
        destination.height < view.height)
        return false;
    return dk1_ppu_render_vram_bg(
        &runtime->vram, &scene->cgram, &runtime->layout,
        view.camera_x, view.camera_y, destination, false);
}

uint64_t dk1_dynamic_bg1_runtime_signature(
    const Dk1DynamicBg1Runtime *runtime) {
    uint64_t hash = 0u;
    if (runtime == NULL)
        return 0u;
    hash = dk1_fnv1a64(&runtime->layout, sizeof(runtime->layout), hash);
    hash = dk1_fnv1a64(&runtime->last, sizeof(runtime->last), hash);
    hash = dk1_fnv1a64(&runtime->update_count,
                       sizeof(runtime->update_count), hash);
    hash = dk1_fnv1a64(&runtime->map_columns_uploaded,
                       sizeof(runtime->map_columns_uploaded), hash);
    hash = dk1_fnv1a64(&runtime->tile_columns_uploaded,
                       sizeof(runtime->tile_columns_uploaded), hash);
    hash = dk1_fnv1a64(&runtime->tile_words_uploaded,
                       sizeof(runtime->tile_words_uploaded), hash);
    hash = dk1_fnv1a64(&runtime->generation,
                       sizeof(runtime->generation), hash);
    hash = dk1_fnv1a64(&runtime->ready, sizeof(runtime->ready), hash);
    return dk1_fnv1a64(runtime->vram.bytes,
                       sizeof(runtime->vram.bytes), hash);
}
