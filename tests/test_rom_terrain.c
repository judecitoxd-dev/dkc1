#include <assert.h>
#include <stdlib.h>
#include "dk1/rom_terrain.h"

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
    Dk1RomTerrainView view;
    Dk1TerrainSample sample;
    uint16_t flags[64] = {0};
    assert(bytes != NULL);
    assert(dk1_rom_image_init(&rom, bytes, DK1_EXPECTED_ROM_SIZE));
    config.map_snes = 0xC01000u;
    config.collision_blocks_snes = 0xC02000u;
    config.block_count = 2u;
    put16(bytes, config.map_snes + 15u * 2u, 1u);
    put16(bytes, config.collision_blocks_snes + 4u, 39u);
    flags[39] = 0x0012u;
    assert(dk1_rom_terrain_view_init(&view, &rom, config, 1u, flags));
    assert(dk1_rom_terrain_sample_cell(&view, 5, 0, &sample));
    assert(sample.floor_y == 5);
    assert(sample.shape_id == 39u);
    assert((sample.attributes & 0x0012u) == 0x0012u);
    assert(dk1_rom_terrain_find_floor(&view, 5, 0, 1u, &sample));
    assert(!dk1_rom_terrain_sample_cell(&view, 40, 0, &sample));
    free(bytes);
    return 0;
}
