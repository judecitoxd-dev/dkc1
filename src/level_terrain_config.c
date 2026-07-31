#include "dk1/level_terrain_config.h"

size_t dk1_level_terrain_config_count(void) {
    return DK1_LEVEL_TERRAIN_CONFIG_COUNT;
}

bool dk1_level_terrain_config_read(const Dk1RomImage *rom,
                                   uint16_t config_id,
                                   Dk1LevelTerrainConfig *config) {
    const uint32_t x = (uint32_t)config_id * 3u;
    const uint32_t y = (uint32_t)config_id * 2u;
    uint16_t map_address = 0u;
    uint8_t map_bank = 0u;
    uint16_t visual_base = 0u;
    uint8_t visual_bank = 0u;
    uint16_t collision_address = 0u;
    uint8_t collision_bank = 0u;
    uint16_t callback = 0u;
    if (rom == NULL || config == NULL || config_id >= DK1_LEVEL_TERRAIN_CONFIG_COUNT) {
        return false;
    }
    if (!dk1_rom_read_u16(rom, 0x818B94u + x, &map_address) ||
        !dk1_rom_read_u8(rom, 0x818B96u + x, &map_bank) ||
        !dk1_rom_read_u16(rom, 0x818BBEu + x, &visual_base) ||
        !dk1_rom_read_u8(rom, 0x818BC0u + x, &visual_bank) ||
        !dk1_rom_read_u16(rom, 0x818C04u + x, &collision_address) ||
        !dk1_rom_read_u8(rom, 0x818C06u + x, &collision_bank) ||
        !dk1_rom_read_u16(rom, 0x818BE8u + y, &config->stream_vram_base) ||
        !dk1_rom_read_u16(rom, 0x818C2Eu + y, &config->block_count) ||
        !dk1_rom_read_u16(rom, 0x818C4Au + y, &callback)) {
        return false;
    }
    if (visual_bank == 0u) visual_bank = 0x80u;
    config->map_snes = ((uint32_t)map_bank << 16u) | map_address;
    config->visual_bank = visual_bank;
    config->visual_block_base = visual_base;
    config->collision_blocks_snes = ((uint32_t)collision_bank << 16u) |
                                    collision_address;
    config->terrain_callback_snes = 0x810000u | callback;
    return true;
}
