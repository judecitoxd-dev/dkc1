#include <assert.h>
#include <stdlib.h>
#include "dk1/level_map.h"

static void put16(uint8_t *rom, uint32_t pc, uint16_t value) {
    size_t offset = 0u;
    assert(dk1_hirom_offset(pc, DK1_EXPECTED_ROM_SIZE, &offset));
    rom[offset] = (uint8_t)value;
    rom[offset + 1u] = (uint8_t)(value >> 8u);
}

int main(void) {
    uint8_t *bytes = (uint8_t *)calloc(DK1_EXPECTED_ROM_SIZE, 1u);
    Dk1RomImage rom;
    Dk1LevelTerrainConfig config = {0};
    uint16_t tiles[4u * 64u] = {0};
    uint16_t cell = 0u;
    size_t i = 0u;
    assert(bytes != NULL);
    assert(dk1_rom_image_init(&rom, bytes, DK1_EXPECTED_ROM_SIZE));
    config.map_snes = 0xC01000u;
    config.visual_bank = 0xC0u;
    config.visual_block_base = 0x3000u;
    config.block_count = 2u;
    for (i = 0u; i < 16u; ++i) put16(bytes, config.map_snes + (uint32_t)i * 2u, 1u);
    for (i = 0u; i < 16u; ++i) put16(bytes, 0xC03020u + (uint32_t)i * 2u, (uint16_t)i);
    assert(dk1_level_map_cell(&rom, &config, 0u, 0u, &cell));
    assert(cell == 1u);
    assert(dk1_level_map_expand_tiles(&rom, &config, 0u, 1u, tiles, 4u, 64u));
    assert(tiles[0] == 0u);
    assert(tiles[3] == 3u);
    assert(tiles[4u * 3u] == 12u);
    assert(tiles[4u * 4u] == 0u);
    free(bytes);
    return 0;
}
