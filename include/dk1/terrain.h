#ifndef DK1_TERRAIN_H
#define DK1_TERRAIN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/rom_image.h"

#define DK1_TERRAIN_ROWS 16u
#define DK1_TERRAIN_CELL_SIZE 32u
#define DK1_TERRAIN_HALF_SIZE 16u
#define DK1_TERRAIN_SHAPE_COUNT 64u

typedef struct Dk1TerrainLayout {
    const uint16_t *column_map;
    size_t column_count;
    const uint16_t *block_halves;
    size_t block_count;
    const uint16_t *shape_flags;
} Dk1TerrainLayout;

typedef struct Dk1TerrainSample {
    bool hit;
    int32_t floor_y;
    uint16_t attributes;
    uint16_t block_index;
    uint8_t shape_id;
    uint8_t local_x;
} Dk1TerrainSample;

bool dk1_terrain_shape_flags_from_rom(const Dk1RomImage *rom, uint16_t output[DK1_TERRAIN_SHAPE_COUNT]);
bool dk1_terrain_shape_height(uint8_t shape_id, uint8_t local_x, int16_t *height);
bool dk1_terrain_sample_cell(const Dk1TerrainLayout *layout, int32_t world_x, int32_t cell_y, Dk1TerrainSample *sample);
bool dk1_terrain_find_floor(const Dk1TerrainLayout *layout, int32_t world_x, int32_t probe_y, size_t max_cells, Dk1TerrainSample *sample);

#endif
