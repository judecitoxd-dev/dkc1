#include "dk1/rom_terrain.h"

bool dk1_rom_terrain_view_init(Dk1RomTerrainView *view,
                               const Dk1RomImage *rom,
                               Dk1LevelTerrainConfig config,
                               size_t column_count,
                               const uint16_t *shape_flags) {
    if (view == NULL || rom == NULL || rom->data == NULL || column_count == 0u ||
        config.block_count == 0u) return false;
    view->rom = rom;
    view->config = config;
    view->column_count = column_count;
    view->shape_flags = shape_flags;
    return true;
}

bool dk1_rom_terrain_sample_cell(const Dk1RomTerrainView *view,
                                 int32_t world_x, int32_t cell_y,
                                 Dk1TerrainSample *sample) {
    size_t column = 0u;
    size_t row = 0u;
    size_t map_index = 0u;
    uint16_t cell = 0u;
    uint16_t block_index = 0u;
    uint8_t x32 = 0u;
    size_t half = 0u;
    uint8_t local_x = 0u;
    uint16_t descriptor = 0u;
    uint8_t descriptor_byte = 0u;
    uint8_t shape_id = 0u;
    int16_t height = 0;
    if (sample == NULL) return false;
    *sample = (Dk1TerrainSample){0};
    if (view == NULL || view->rom == NULL || world_x < 0 || cell_y < 0 ||
        cell_y >= 512) return false;
    column = (size_t)world_x / DK1_TERRAIN_CELL_SIZE;
    if (column >= view->column_count) return false;
    row = 15u - ((size_t)cell_y / DK1_TERRAIN_CELL_SIZE);
    map_index = column * DK1_TERRAIN_ROWS + row;
    if (!dk1_rom_read_u16(view->rom,
                          view->config.map_snes + (uint32_t)(map_index * 2u),
                          &cell) || cell == 0u) return false;
    block_index = (uint16_t)(cell & 0x3FFFu);
    if (block_index >= view->config.block_count) return false;
    x32 = (uint8_t)((uint32_t)world_x & 0x1Fu);
    if ((cell & 0x4000u) != 0u) x32 ^= 0x1Fu;
    half = (x32 & 0x10u) != 0u ? 1u : 0u;
    local_x = (uint8_t)(x32 & 0x0Fu);
    if (!dk1_rom_read_u16(view->rom,
                          view->config.collision_blocks_snes +
                              (uint32_t)block_index * 4u + (uint32_t)half * 2u,
                          &descriptor)) return false;
    if ((descriptor & 0x0080u) != 0u) local_x ^= 0x0Fu;
    if ((cell & 0x4000u) != 0u) descriptor ^= 0x0080u;
    descriptor_byte = (uint8_t)descriptor;
    shape_id = (uint8_t)(descriptor_byte & 0x3Fu);
    if (shape_id == 0u || !dk1_terrain_shape_height(shape_id, local_x, &height) ||
        height < 0) return false;
    if (height > 31) height = 31;
    sample->hit = true;
    sample->floor_y = (cell_y & ~31) | height;
    sample->attributes = (uint16_t)(descriptor_byte & 0xC0u);
    if (view->shape_flags != NULL) {
        sample->attributes |= (uint16_t)(view->shape_flags[shape_id] & 0x801Fu);
    }
    sample->block_index = block_index;
    sample->shape_id = shape_id;
    sample->local_x = local_x;
    return true;
}

bool dk1_rom_terrain_find_floor(const Dk1RomTerrainView *view,
                                int32_t world_x, int32_t probe_y,
                                size_t max_cells,
                                Dk1TerrainSample *sample) {
    size_t i = 0u;
    int32_t cell_y = 0;
    if (sample == NULL || probe_y < 0) return false;
    cell_y = probe_y & ~31;
    for (i = 0u; i <= max_cells && cell_y < 512; ++i, cell_y += 32) {
        if (dk1_rom_terrain_sample_cell(view, world_x, cell_y, sample)) return true;
    }
    *sample = (Dk1TerrainSample){0};
    return false;
}

bool dk1_rom_terrain_point_solid(const Dk1RomTerrainView *view,
                                 int32_t world_x, int32_t world_y,
                                 Dk1TerrainSample *sample) {
    Dk1TerrainSample local;
    int32_t cell_y;
    if (sample == NULL) sample = &local;
    *sample = (Dk1TerrainSample){0};
    if (view == NULL || world_x < 0 || world_y < 0 || world_y >= 512) return false;
    cell_y = world_y & ~(int32_t)(DK1_TERRAIN_CELL_SIZE - 1u);
    if (!dk1_rom_terrain_sample_cell(view, world_x, cell_y, sample)) return false;
    if (world_y > sample->floor_y) {
        *sample = (Dk1TerrainSample){0};
        return false;
    }
    return true;
}
