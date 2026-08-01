#ifndef DK1_SCENE_MEMORY_H
#define DK1_SCENE_MEMORY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/scene_recipe.h"
#include "dk1/scene_package_plan.h"
#include "dk1/vram_image.h"
#include "dk1/cgram_image.h"
#include "dk1/ppu_preset.h"
#include "dk1/level_terrain_config.h"
#include "dk1/level_camera_bounds.h"

typedef struct Dk1SceneAssetStats {
    size_t package_count;
    size_t package_records;
    size_t direct_records;
    size_t compressed_records;
    size_t dma_bytes;
    size_t decompressed_bytes;
    size_t palette_uploads;
    size_t palette_colors;
    size_t nonzero_vram_bytes;
    size_t nonzero_palette_colors;
    uint64_t vram_signature;
    uint64_t palette_signature;
    bool textures_loaded;
    bool palettes_loaded;
} Dk1SceneAssetStats;

typedef struct Dk1SceneMemory {
    Dk1SceneRecipe recipe;
    Dk1ScenePackagePlan packages;
    Dk1PpuPreset ppu;
    Dk1LevelTerrainConfig terrain;
    Dk1LevelCameraBounds camera;
    Dk1VramImage vram;
    Dk1CgramImage cgram;
    Dk1SceneAssetStats assets;
} Dk1SceneMemory;

bool dk1_scene_memory_load(const Dk1RomImage *rom,
                           uint16_t level_id,
                           bool special_assets,
                           bool state_package,
                           Dk1SceneMemory *scene);

#endif
