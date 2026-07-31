#include "dk1/level_map.h"

bool dk1_level_map_cell(const Dk1RomImage *rom,
                        const Dk1LevelTerrainConfig *config,
                        size_t column, size_t row_from_top,
                        uint16_t *cell) {
    const size_t index = row_from_top * 64u + column;
    if (rom == NULL || config == NULL || cell == NULL || row_from_top >= 16u) {
        return false;
    }
    return dk1_rom_read_u16(rom,
                            config->map_snes + (uint32_t)(index * 2u), cell);
}

bool dk1_level_map_expand_tiles(const Dk1RomImage *rom,
                                const Dk1LevelTerrainConfig *config,
                                size_t first_column, size_t column_count,
                                uint16_t *tiles, size_t tile_stride,
                                size_t tile_height) {
    size_t column = 0u;
    if (rom == NULL || config == NULL || tiles == NULL || column_count == 0u ||
        tile_stride < column_count * 4u || tile_height < 64u) return false;
    for (column = 0u; column < column_count; ++column) {
        size_t row = 0u;
        for (row = 0u; row < 16u; ++row) {
            uint16_t cell = 0u;
            uint16_t metatile[16];
            size_t my = 0u;
            if (!dk1_level_map_cell(rom, config, first_column + column, row, &cell) ||
                !dk1_level_metatile_expand(rom, config, cell, metatile)) return false;
            for (my = 0u; my < 4u; ++my) {
                size_t mx = 0u;
                for (mx = 0u; mx < 4u; ++mx) {
                    tiles[(row * 4u + my) * tile_stride + column * 4u + mx] =
                        metatile[my * 4u + mx];
                }
            }
        }
    }
    return true;
}
