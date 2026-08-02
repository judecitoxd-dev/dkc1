#include "dk1/preview_asset_warmup.h"
#include <string.h>
#include "dk1/scene_signature.h"

bool dk1_preview_asset_unblank_scene(Dk1SceneMemory *scene) {
    const size_t index = (size_t)(0x2100u - DK1_PPU_REGISTER_FIRST);
    uint8_t display;
    bool changed;

    if (scene == NULL || index >= DK1_PPU_REGISTER_COUNT)
        return false;
    display = scene->ppu.values[index];
    changed = !scene->ppu.written[index] ||
              (display & 0x80u) != 0u ||
              (display & 0x0Fu) == 0u;
    if (!changed)
        return false;
    if (!scene->ppu.written[index]) {
        scene->ppu.written[index] = true;
        ++scene->ppu.write_count;
    }
    scene->ppu.values[index] = 0x0Fu;
    return true;
}

static size_t visible_pixel_count(Dk1RgbaSurface surface) {
    size_t x;
    size_t y;
    size_t count = 0u;

    for (y = 0u; y < surface.height; ++y) {
        const Dk1Rgba8 *row = surface.pixels + y * surface.stride;
        for (x = 0u; x < surface.width; ++x) {
            if (row[x].r != 0u || row[x].g != 0u || row[x].b != 0u)
                ++count;
        }
    }
    return count;
}

static uint64_t visible_frame_signature(Dk1RgbaSurface surface) {
    uint64_t signature = 0u;
    size_t y;

    for (y = 0u; y < surface.height; ++y) {
        const Dk1Rgba8 *row = surface.pixels + y * surface.stride;
        signature = dk1_fnv1a64(
            row, surface.width * sizeof(*row), signature);
    }
    return signature;
}

bool dk1_preview_asset_warmup(
    const Dk1RomImage *rom,
    Dk1SceneMemory *scene,
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

    local.display_unblanked = dk1_preview_asset_unblank_scene(scene);
    memset(scratch_surface.pixels, 0,
           scratch_surface.stride * scratch_surface.height *
               sizeof(*scratch_surface.pixels));
    if (!dk1_software_frontend_render(
            rom, scene, frontend, scratch_surface))
        return false;

    local.visible_pixels = visible_pixel_count(scratch_surface);
    local.frame_signature = visible_frame_signature(scratch_surface);
    local.visible_frame_ready =
        local.visible_pixels != 0u && local.frame_signature != 0u;
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
    return local.visible_frame_ready && local.player_visual_ready &&
           local.player_dma_records != 0u &&
           local.player_dma_bytes != 0u;
}
