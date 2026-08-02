#ifndef DK1_PREVIEW_ASSET_WARMUP_H
#define DK1_PREVIEW_ASSET_WARMUP_H

#include <stdbool.h>
#include <stddef.h>
#include "dk1/software_frontend.h"

typedef struct Dk1PreviewAssetWarmupStats {
    size_t scene_packages;
    size_t scene_dma_bytes;
    size_t scene_decompressed_bytes;
    size_t player_dma_records;
    size_t player_dma_bytes;
    size_t gnawty_visuals;
    size_t barrel_visuals;
    bool scene_textures_ready;
    bool scene_palettes_ready;
    bool dynamic_bg1_ready;
    bool player_visual_ready;
} Dk1PreviewAssetWarmupStats;

/* Renders one hidden frame before the intro. This validates the scene package
 * load and materializes the first player/enemy/barrel frame uploads so entering
 * the first level does not need to perform its initial texture work. */
bool dk1_preview_asset_warmup(
    const Dk1RomImage *rom,
    const Dk1SceneMemory *scene,
    Dk1SoftwareFrontend *frontend,
    Dk1RgbaSurface scratch_surface,
    Dk1PreviewAssetWarmupStats *stats);

#endif
