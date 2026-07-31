#include <assert.h>
#include <stdlib.h>
#include "dk1/level_metatile.h"

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
    uint16_t output[16];
    size_t i = 0u;
    assert(bytes != NULL);
    assert(dk1_rom_image_init(&rom, bytes, DK1_EXPECTED_ROM_SIZE));
    config.visual_bank = 0xC0u;
    config.visual_block_base = 0x3000u;
    config.block_count = 2u;
    for (i = 0u; i < 16u; ++i) {
        put16(bytes, 0xC03020u + (uint32_t)i * 2u, (uint16_t)(0x0100u + i));
    }
    assert(dk1_level_metatile_expand(&rom, &config, 1u, output));
    for (i = 0u; i < 16u; ++i) assert(output[i] == (uint16_t)(0x0100u + i));
    assert(dk1_level_metatile_expand(&rom, &config, 0x4001u, output));
    assert(output[0] == (uint16_t)(0x0103u ^ 0x4000u));
    assert(output[3] == (uint16_t)(0x0100u ^ 0x4000u));
    assert(dk1_level_metatile_expand(&rom, &config, 0x8001u, output));
    assert(output[0] == (uint16_t)(0x010Cu ^ 0x8000u));
    assert(output[15] == (uint16_t)(0x0103u ^ 0x8000u));
    free(bytes);
    return 0;
}
