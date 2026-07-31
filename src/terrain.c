#include "dk1/terrain.h"

static const uint8_t CURVE_86E5[16] = {
    1u, 2u, 2u, 3u, 4u, 4u, 5u, 6u,
    6u, 7u, 8u, 8u, 9u, 10u, 10u, 11u,
};

bool dk1_terrain_shape_flags_from_rom(const Dk1RomImage *rom,
                                      uint16_t output[DK1_TERRAIN_SHAPE_COUNT]) {
    size_t i = 0u;
    if (rom == NULL || output == NULL) return false;
    for (i = 0u; i < DK1_TERRAIN_SHAPE_COUNT; ++i) {
        if (!dk1_rom_read_u16(rom, 0x818409u + (uint32_t)i, &output[i])) return false;
        output[i] &= 0x801Fu;
    }
    return true;
}

bool dk1_terrain_shape_height(uint8_t shape_id, uint8_t local_x,
                              int16_t *height) {
    int value = 0;
    const int x = (int)(local_x & 0x0Fu);
    if (height == NULL || shape_id >= 63u) return false;
    switch (shape_id) {
        case 0u: value = -1; break;
        case 1u: value = 31; break;
        case 2u: value = 27; break;
        case 3u: value = 23; break;
        case 4u: value = 19; break;
        case 5u: value = 15; break;
        case 6u: value = 11; break;
        case 7u: value = 7; break;
        case 8u: value = 3; break;
        case 9u: value = x / 4; break;
        case 10u: value = x / 4 + 4; break;
        case 11u: value = x / 4 + 8; break;
        case 12u: value = x / 4 + 12; break;
        case 13u: value = x / 4 + 16; break;
        case 14u: value = x / 4 + 20; break;
        case 15u: value = x / 4 + 24; break;
        case 16u: value = x / 4 + 28; break;
        case 17u: value = x / 2 - 4; break;
        case 18u: value = x / 2; break;
        case 19u: value = x / 2 + 4; break;
        case 20u: value = x / 2 + 8; break;
        case 21u: value = x / 2 + 12; break;
        case 22u: value = x / 2 + 16; break;
        case 23u: value = x / 2 + 20; break;
        case 24u: value = x / 2 + 24; break;
        case 25u: value = x / 2 + 28; break;
        case 26u: value = (int)CURVE_86E5[x] - 8; break;
        case 27u: value = (int)CURVE_86E5[x] - 4; break;
        case 28u: value = (int)CURVE_86E5[x]; break;
        case 29u: value = (int)CURVE_86E5[x] + 4; break;
        case 30u: value = (int)CURVE_86E5[x] + 8; break;
        case 31u: value = (int)CURVE_86E5[x] + 12; break;
        case 32u: value = (int)CURVE_86E5[x] + 16; break;
        case 33u: value = (int)CURVE_86E5[x] + 20; break;
        case 34u: value = (int)CURVE_86E5[x] + 24; break;
        case 35u: value = (int)CURVE_86E5[x] + 28; break;
        case 36u: value = x - 12; break;
        case 37u: value = x - 8; break;
        case 38u: value = x - 4; break;
        case 39u: value = x; break;
        case 40u: value = x + 4; break;
        case 41u: value = x + 8; break;
        case 42u: value = x + 12; break;
        case 43u: value = x + 16; break;
        case 44u: value = x + 20; break;
        case 45u: value = x + 24; break;
        case 46u: value = x + 28; break;
        case 47u: value = x * 2 - 24; break;
        case 48u: value = x * 2 - 16; break;
        case 49u: value = x * 2 - 8; break;
        case 50u: value = x * 2; break;
        case 51u: value = x * 2 + 8; break;
        case 52u: value = x * 2 + 16; break;
        case 53u: value = x * 2 + 24; break;
        case 54u: value = x * 4 - 48; break;
        case 55u: value = x * 4 - 32; break;
        case 56u: value = x * 4 - 16; break;
        case 57u: value = x * 4; break;
        case 58u: value = x * 4 + 16; break;
        case 59u:
            value = x * 4 - 16;
            if (value >= 15) value = 15;
            break;
        case 60u:
            value = x * 4 - 16;
            if (value < 15) value = 15;
            break;
        case 61u: value = 31; break;
        case 62u: value = 15; break;
        default: return false;
    }
    *height = (int16_t)value;
    return true;
}

static bool layout_valid(const Dk1TerrainLayout *layout) {
    return layout != NULL && layout->column_map != NULL &&
           layout->column_count > 0u && layout->block_halves != NULL &&
           layout->block_count > 0u;
}

bool dk1_terrain_sample_cell(const Dk1TerrainLayout *layout,
                             int32_t world_x, int32_t cell_y,
                             Dk1TerrainSample *sample) {
    size_t column = 0u;
    size_t row = 0u;
    size_t map_index = 0u;
    uint16_t cell = 0u;
    uint16_t block_index = 0u;
    uint8_t x32 = 0u;
    uint8_t local_x = 0u;
    size_t half = 0u;
    uint16_t descriptor = 0u;
    uint8_t descriptor_byte = 0u;
    uint8_t shape_id = 0u;
    int16_t height = 0;
    if (sample == NULL) return false;
    *sample = (Dk1TerrainSample){0};
    if (!layout_valid(layout) || world_x < 0 || cell_y < 0 || cell_y >= 512) return false;

    column = (size_t)world_x / DK1_TERRAIN_CELL_SIZE;
    if (column >= layout->column_count) return false;
    row = 15u - ((size_t)cell_y / DK1_TERRAIN_CELL_SIZE);
    map_index = column * DK1_TERRAIN_ROWS + row;
    cell = layout->column_map[map_index];
    if (cell == 0u) return false;
    block_index = (uint16_t)(cell & 0x3FFFu);
    if ((size_t)block_index >= layout->block_count) return false;

    x32 = (uint8_t)((uint32_t)world_x & 0x1Fu);
    if ((cell & 0x4000u) != 0u) x32 ^= 0x1Fu;
    half = (x32 & 0x10u) != 0u ? 1u : 0u;
    local_x = (uint8_t)(x32 & 0x0Fu);
    descriptor = layout->block_halves[(size_t)block_index * 2u + half];
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
    if (layout->shape_flags != NULL) {
        sample->attributes |= (uint16_t)(layout->shape_flags[shape_id] & 0x801Fu);
    }
    sample->block_index = block_index;
    sample->shape_id = shape_id;
    sample->local_x = local_x;
    return true;
}

bool dk1_terrain_find_floor(const Dk1TerrainLayout *layout,
                            int32_t world_x, int32_t probe_y,
                            size_t max_cells, Dk1TerrainSample *sample) {
    size_t i = 0u;
    int32_t cell_y = 0;
    if (sample == NULL || probe_y < 0) return false;
    cell_y = probe_y & ~31;
    for (i = 0u; i <= max_cells && cell_y < 512; ++i, cell_y += 32) {
        if (dk1_terrain_sample_cell(layout, world_x, cell_y, sample)) return true;
    }
    *sample = (Dk1TerrainSample){0};
    return false;
}
