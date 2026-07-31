#include <assert.h>
#include <stdlib.h>
#include "dk1/level_diagnostic.h"

static void put16(uint8_t *rom, uint32_t pc, uint16_t value) {
    size_t offset = 0u;
    assert(dk1_hirom_offset(pc, DK1_EXPECTED_ROM_SIZE, &offset));
    rom[offset] = (uint8_t)value;
    rom[offset + 1u] = (uint8_t)(value >> 8u);
}

int main(void) {
    uint8_t *bytes = (uint8_t *)calloc(DK1_EXPECTED_ROM_SIZE, 1u);
    Dk1Rgba8 *pixels = (Dk1Rgba8 *)calloc(32u * 512u, sizeof(*pixels));
    Dk1RomImage rom;
    Dk1LevelTerrainConfig config = {0};
    Dk1RgbaSurface surface = {pixels, 32u, 512u, 32u};
    size_t i = 0u;
    assert(bytes != NULL && pixels != NULL);
    assert(dk1_rom_image_init(&rom, bytes, DK1_EXPECTED_ROM_SIZE));
    config.map_snes = 0xC01000u;
    config.visual_bank = 0xC0u;
    config.visual_block_base = 0x3000u;
    config.collision_blocks_snes = 0xC02000u;
    config.block_count = 2u;
    for (i = 0u; i < 16u; ++i) put16(bytes, config.map_snes + (uint32_t)i * 2u, 1u);
    for (i = 0u; i < 16u; ++i) put16(bytes, 0xC03020u + (uint32_t)i * 2u, (uint16_t)(1u + i));
    put16(bytes, config.collision_blocks_snes + 4u, 1u);
    assert(dk1_level_diagnostic_render(&rom, config, 0u, 1u, surface));
    assert(pixels[0].a == 255u);
    assert(pixels[31u * 32u].r == 255u);
    free(pixels);
    free(bytes);
    return 0;
}
