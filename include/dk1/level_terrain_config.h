#ifndef DK1_LEVEL_TERRAIN_CONFIG_H
#define DK1_LEVEL_TERRAIN_CONFIG_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/rom_image.h"

#define DK1_LEVEL_TERRAIN_CONFIG_COUNT 14u

typedef struct Dk1LevelTerrainConfig {
    uint32_t map_snes;
    uint8_t visual_bank;
    uint16_t visual_block_base;
    uint32_t collision_blocks_snes;
    uint16_t stream_vram_base;
    uint16_t block_count;
    uint32_t terrain_callback_snes;
} Dk1LevelTerrainConfig;

/* Clean-room translation of the split records consumed by $81:8C67-$81:8CAF. */
size_t dk1_level_terrain_config_count(void);
bool dk1_level_terrain_config_read(const Dk1RomImage *rom,
                                   uint16_t config_id,
                                   Dk1LevelTerrainConfig *config);

#endif
