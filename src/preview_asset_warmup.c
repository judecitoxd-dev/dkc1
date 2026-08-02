#include "dk1/preview_asset_warmup.h"
#include <string.h>

bool dk1_preview_asset_warmup(
    const Dk1RomImage *rom,
    const Dk1SceneMemory *scene,
    Dk1SoftwareFrontend *frontend,
    Dk1RgbaSurface scratch_surface,
    Dk1PreviewAssetWarmupStats *stats) {
    Dk1PreviewAssetWarmupStats local;
    size_t i;

    memset(&local, 0, sizeof(local));
    if (rom == NULL || scene == NULL || frontend == NULL ||
        scratch_surface.pixels == NULL ||
        scratch_surface.width != frontend->runtime.view.width ||
        scratch_surface.height != frontend->runtime.view.height ||
        scratch_surface.stride < scratch_surface.width)
        return false;

    local.scene_packages = scene->assets.package_count;
    local.scene_dma_bytes = scene->assets.dma_bytes;
    local.scene_decompressed_bytes = scene->assets.decompressed_bytes;
    local.scene_textures_ready = scene->assets.textures_loaded;
    local.scene_palettes_ready = scene->assets.palettes_loaded;
    local.dynamic_bg1_ready =
        frontend->dynamic_bg1 != NULL && frontend->dynamic_bg1->ready;
    if (!local.scene_textures_ready || !local.scene_palettes_ready)
        return false;

    if (!dk1_software_frontend_render(
            rom, scene, frontend, scratch_surface))
        return false;

    local.player_visual_ready = frontend->player_visual_ready;
    local.player_dma_records = frontend->player_visual.dma_records;
    local.player_dma_bytes = frontend->player_visual.dma_bytes;

    if (frontend->gnawty_ready && frontend->gnawty.active &&
        frontend->gnawty.visual_ready)
        ++local.gnawty_visuals;
    for (i = 0u; i < DK1_SOFTWARE_FRONTEND_EXTRA_GNAWTIES; ++i) {
        const Dk1SoftwareGnawtySlot *slot =
            &frontend->additional_gnawties[i];
        if (slot->ready && slot->runtime != NULL &&
            slot->runtime->active && slot->runtime->visual_ready)
            ++local.gnawty_visuals;
    }

    if (frontend->barrel_ready && frontend->barrel.live.active &&
        frontend->barrel.visual_ready)
        ++local.barrel_visuals;
    for (i = 0u; i < DK1_LEVEL_BARREL_POOL_CAPACITY; ++i) {
        const Dk1LevelBarrelSlot *slot = &frontend->streamed_barrels.slots[i];
        if (slot->ready && slot->runtime != NULL &&
            slot->runtime->live.active && slot->runtime->visual_ready)
            ++local.barrel_visuals;
    }

    if (stats != NULL)
        *stats = local;
    return local.player_visual_ready && local.player_dma_records != 0u &&
           local.player_dma_bytes != 0u;
}
