#include "dk1/level_metatile.h"

bool dk1_level_metatile_row(const Dk1RomImage *rom,
                            const Dk1LevelTerrainConfig *config,
                            uint16_t map_cell,
                            uint8_t output_row,
                            uint16_t output[DK1_METATILE_WIDTH]) {
    const uint16_t block = (uint16_t)(map_cell & 0x3FFFu);
    const bool horizontal_flip = (map_cell & 0x4000u) != 0u;
    const bool vertical_flip = (map_cell & 0x8000u) != 0u;
    const uint8_t source_row = vertical_flip ? (uint8_t)(3u - output_row) : output_row;
    uint8_t column = 0u;
    if (rom == NULL || config == NULL || output == NULL || output_row >= 4u) return false;
    for (column = 0u; column < 4u; ++column) {
        const uint8_t source_column = horizontal_flip ? (uint8_t)(3u - column) : column;
        const uint16_t address = (uint16_t)(config->visual_block_base +
                                 (uint16_t)(block * 32u) +
                                 (uint16_t)(source_row * 8u) +
                                 (uint16_t)(source_column * 2u));
        uint16_t word = 0u;
        if (!dk1_rom_read_u16(rom,
                              ((uint32_t)config->visual_bank << 16u) | address,
                              &word)) return false;
        if (horizontal_flip) word ^= 0x4000u;
        if (vertical_flip) word ^= 0x8000u;
        output[column] = word;
    }
    return true;
}

bool dk1_level_metatile_expand(const Dk1RomImage *rom,
                               const Dk1LevelTerrainConfig *config,
                               uint16_t map_cell,
                               uint16_t output[DK1_METATILE_WORD_COUNT]) {
    uint8_t row = 0u;
    if (output == NULL) return false;
    for (row = 0u; row < 4u; ++row) {
        if (!dk1_level_metatile_row(rom, config, map_cell, row,
                                    output + (size_t)row * 4u)) return false;
    }
    return true;
}
