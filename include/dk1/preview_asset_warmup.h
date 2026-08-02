#ifndef DK1_PREVIEW_ASSET_WARMUP_H
#define DK1_PREVIEW_ASSET_WARMUP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/software_frontend.h"

typedef struct Dk1PreviewAssetWarmupStats {
    size_t scene_packages;
    size_t scene_dma_bytes;
    size_t scene_decompressed_bytes;
    size_t player_dma_records;
    size_t player_dma_bytes;
    size_t gnawty_visuals;
    size_t barrel_visuals;
    size_t visible_pixels;
    uint64_t frame_signature;
    bool scene_textures_ready;
    bool scene_palettes_ready;
    bool dynamic_bg1_ready;
    bool player_visual_ready;
    bool display_unblanked;
    bool visible_frame_ready;
} Dk1PreviewAssetWarmupStats;

/* Host previews enter a prepared level without executing the original fade-in
 * callback. If the loaded SNES preset still has INIDISP force-blanked or at
 * brightness zero, expose the prepared scene at full brightness. Returns true
 * when the preset had to be changed. */
bool dk1_preview_asset_unblank_scene(Dk1SceneMemory *scene);

/* Renders one hidden frame before the intro. This validates the scene package
 * load, rejects an all-black output frame and materializes the first
 * player/enemy/barrel frame uploads so entering the first level does not need
 * to perform its initial texture work. */
bool dk1_preview_asset_warmup(
    const Dk1RomImage *rom,
    Dk1SceneMemory *scene,
    Dk1SoftwareFrontend *frontend,
    Dk1RgbaSurface scratch_surface,
    Dk1PreviewAssetWarmupStats *stats);

#endif
