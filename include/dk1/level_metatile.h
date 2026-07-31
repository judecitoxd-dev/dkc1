#ifndef DK1_LEVEL_METATILE_H
#define DK1_LEVEL_METATILE_H

#include <stdbool.h>
#include <stdint.h>
#include "dk1/level_terrain_config.h"

#define DK1_METATILE_WIDTH 4u
#define DK1_METATILE_HEIGHT 4u
#define DK1_METATILE_WORD_COUNT 16u

bool dk1_level_metatile_row(const Dk1RomImage *rom,
                            const Dk1LevelTerrainConfig *config,
                            uint16_t map_cell,
                            uint8_t output_row,
                            uint16_t output[DK1_METATILE_WIDTH]);
bool dk1_level_metatile_expand(const Dk1RomImage *rom,
                               const Dk1LevelTerrainConfig *config,
                               uint16_t map_cell,
                               uint16_t output[DK1_METATILE_WORD_COUNT]);

#endif
