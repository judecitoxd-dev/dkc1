#include "dk1/scene_memory.h"
#include <string.h>

static uint64_t hash_bytes(const void *data, size_t size) {
    const uint8_t *bytes = (const uint8_t *)data;
    uint64_t hash = UINT64_C(1469598103934665603);
    size_t i;
    for (i = 0u; i < size; ++i) {
        hash ^= bytes[i];
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

static bool palette_apply(const Dk1RomImage *rom,
                          const Dk1ScenePaletteUpload *upload,
                          Dk1CgramImage *cgram,
                          size_t *loaded_colors) {
    const size_t bytes = (size_t)upload->x_count * 8u;
    const size_t colors = bytes / 2u;
    size_t i;
    if (loaded_colors != NULL)
        *loaded_colors = 0u;
    if (upload->mode == DK1_PALETTE_UPLOAD_NORMAL) {
        if (!dk1_cgram_upload_b9(rom, upload->source_b9,
                                 upload->cgram_start, upload->x_count,
                                 cgram))
            return false;
        if (loaded_colors != NULL)
            *loaded_colors = colors;
        return true;
    }
    if ((size_t)upload->cgram_start + colors > DK1_CGRAM_COLOR_COUNT)
        return false;
    for (i = 0u; i < colors; ++i) {
        uint8_t low = 0u;
        uint8_t high = 0u;
        const uint16_t source =
            (uint16_t)(upload->source_b9 + (uint16_t)(i * 2u));
        if (!dk1_rom_read_u8(rom, 0xB90000u | source, &low) ||
            !dk1_rom_read_u8(rom,
                             0xB90000u |
                             (uint16_t)(source + 1u),
                             &high))
            return false;
        cgram->colors[upload->cgram_start + i] =
            (uint16_t)(uint8_t)~low |
            ((uint16_t)(uint8_t)~high << 8u);
    }
    if (loaded_colors != NULL)
        *loaded_colors = colors;
    return true;
}

static void finish_asset_stats(Dk1SceneMemory *scene) {
    size_t i;
    for (i = 0u; i < DK1_VRAM_BYTE_COUNT; ++i) {
        if (scene->vram.bytes[i] != 0u)
            ++scene->assets.nonzero_vram_bytes;
    }
    for (i = 0u; i < DK1_CGRAM_COLOR_COUNT; ++i) {
        if ((scene->cgram.colors[i] & 0x7FFFu) != 0u)
            ++scene->assets.nonzero_palette_colors;
    }
    scene->assets.vram_signature =
        hash_bytes(scene->vram.bytes, sizeof(scene->vram.bytes));
    scene->assets.palette_signature =
        hash_bytes(scene->cgram.colors, sizeof(scene->cgram.colors));
    scene->assets.textures_loaded =
        scene->assets.package_count != 0u &&
        scene->assets.package_records != 0u &&
        scene->assets.nonzero_vram_bytes != 0u;
    scene->assets.palettes_loaded =
        scene->assets.palette_uploads != 0u &&
        scene->assets.nonzero_palette_colors != 0u;
}

bool dk1_scene_memory_load(const Dk1RomImage *rom,
                           uint16_t level_id,
                           bool special_assets,
                           bool state_package,
                           Dk1SceneMemory *scene) {
    size_t i;
    if (rom == NULL || scene == NULL)
        return false;
    memset(scene, 0, sizeof(*scene));
    if (!dk1_scene_recipe_for_level(rom, level_id, &scene->recipe) ||
        !dk1_scene_package_plan(level_id,
                                scene->recipe.primary_vram_package,
                                special_assets, state_package,
                                &scene->packages) ||
        !dk1_ppu_preset_read(rom, scene->recipe.ppu_preset,
                             &scene->ppu) ||
        !dk1_level_terrain_config_read(rom,
                                       scene->recipe.terrain_profile,
                                       &scene->terrain) ||
        !dk1_level_camera_bounds_read(rom, level_id, &scene->camera))
        return false;

    dk1_vram_image_init(&scene->vram);
    scene->assets.package_count = scene->packages.count;
    for (i = 0u; i < scene->packages.count; ++i) {
        Dk1VramLoadStats package_stats;
        if (!dk1_vram_apply_package(rom, scene->packages.ids[i],
                                    &scene->vram, &package_stats))
            return false;
        scene->assets.package_records += package_stats.records;
        scene->assets.direct_records += package_stats.direct_records;
        scene->assets.compressed_records += package_stats.compressed_records;
        scene->assets.dma_bytes += package_stats.dma_bytes;
        scene->assets.decompressed_bytes +=
            package_stats.decompressed_bytes;
    }

    dk1_cgram_init(&scene->cgram);
    scene->assets.palette_uploads = scene->recipe.palette_upload_count;
    for (i = 0u; i < scene->recipe.palette_upload_count; ++i) {
        size_t loaded_colors = 0u;
        if (!palette_apply(rom, &scene->recipe.palette_uploads[i],
                           &scene->cgram, &loaded_colors))
            return false;
        scene->assets.palette_colors += loaded_colors;
    }
    if (scene->recipe.patch_special_palette) {
        scene->cgram.colors[0] = 0x4210u & 0x7FFFu;
        scene->cgram.colors[0x11] = 0x7FFFu;
    }
    finish_asset_stats(scene);
    return true;
}
