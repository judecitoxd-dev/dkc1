#ifndef DK1_LEVEL_MAP_H
#define DK1_LEVEL_MAP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/level_metatile.h"

#define DK1_LEVEL_MAP_ROWS 16u
#define DK1_LEVEL_MAP_TILES_PER_COLUMN 4u
#define DK1_LEVEL_MAP_TILE_ROWS 64u

bool dk1_level_map_cell(const Dk1RomImage *rom,
                        const Dk1LevelTerrainConfig *config,
                        size_t column, size_t row_from_top,
                        uint16_t *cell);
bool dk1_level_map_expand_tiles(const Dk1RomImage *rom,
                                const Dk1LevelTerrainConfig *config,
                                size_t first_column, size_t column_count,
                                uint16_t *tiles, size_t tile_stride,
                                size_t tile_height);

#endif
