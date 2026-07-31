#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "dk1/level_terrain_config.h"

static void put8(uint8_t *rom, uint32_t pc, uint8_t value) {
    size_t offset = 0u;
    assert(dk1_hirom_offset(pc, DK1_EXPECTED_ROM_SIZE, &offset));
    rom[offset] = value;
}
static void put16(uint8_t *rom, uint32_t pc, uint16_t value) {
    put8(rom, pc, (uint8_t)value);
    put8(rom, pc + 1u, (uint8_t)(value >> 8u));
}

int main(void) {
    uint8_t *bytes = (uint8_t *)calloc(DK1_EXPECTED_ROM_SIZE, 1u);
    Dk1RomImage rom;
    Dk1LevelTerrainConfig config;
    const uint32_t x = 2u * 3u;
    const uint32_t y = 2u * 2u;
    assert(bytes != NULL);
    assert(dk1_rom_image_init(&rom, bytes, DK1_EXPECTED_ROM_SIZE));
    put16(bytes, 0x818B94u + x, 0x2345u);
    put8(bytes, 0x818B96u + x, 0xD2u);
    put16(bytes, 0x818BBEu + x, 0x6000u);
    put8(bytes, 0x818BC0u + x, 0x00u);
    put16(bytes, 0x818C04u + x, 0x789Au);
    put8(bytes, 0x818C06u + x, 0xE1u);
    put16(bytes, 0x818BE8u + y, 0x7000u);
    put16(bytes, 0x818C2Eu + y, 0x0042u);
    put16(bytes, 0x818C4Au + y, 0x800Du);
    assert(dk1_level_terrain_config_count() == 14u);
    assert(dk1_level_terrain_config_read(&rom, 2u, &config));
    assert(config.map_snes == 0xD22345u);
    assert(config.visual_bank == 0x80u);
    assert(config.visual_block_base == 0x6000u);
    assert(config.collision_blocks_snes == 0xE1789Au);
    assert(config.stream_vram_base == 0x7000u);
    assert(config.block_count == 0x42u);
    assert(config.terrain_callback_snes == 0x81800Du);
    assert(!dk1_level_terrain_config_read(&rom, 14u, &config));
    free(bytes);
    return 0;
}
