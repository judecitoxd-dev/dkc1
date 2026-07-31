#ifndef DK1_ROM_TERRAIN_H
#define DK1_ROM_TERRAIN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/level_terrain_config.h"
#include "dk1/terrain.h"

typedef struct Dk1RomTerrainView {
    const Dk1RomImage *rom;
    Dk1LevelTerrainConfig config;
    size_t column_count;
    const uint16_t *shape_flags;
} Dk1RomTerrainView;

bool dk1_rom_terrain_view_init(Dk1RomTerrainView *view,
                               const Dk1RomImage *rom,
                               Dk1LevelTerrainConfig config,
                               size_t column_count,
                               const uint16_t *shape_flags);
bool dk1_rom_terrain_sample_cell(const Dk1RomTerrainView *view,
                                 int32_t world_x, int32_t cell_y,
                                 Dk1TerrainSample *sample);
bool dk1_rom_terrain_find_floor(const Dk1RomTerrainView *view,
                                int32_t world_x, int32_t probe_y,
                                size_t max_cells,
                                Dk1TerrainSample *sample);

/*
 * Tests a point against the solid portion below the translated shape surface
 * in its 32-pixel collision cell.  The returned sample keeps the original
 * block, shape and attribute metadata for wall/ceiling/material events.
 */
bool dk1_rom_terrain_point_solid(const Dk1RomTerrainView *view,
                                 int32_t world_x, int32_t world_y,
                                 Dk1TerrainSample *sample);

#endif
