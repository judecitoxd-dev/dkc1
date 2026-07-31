#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "dk1/player_terrain_runtime.h"

static size_t hirom_offset(uint32_t pc) {
    return ((size_t)((pc >> 16u) & 0x3Fu) << 16u) | (uint16_t)pc;
}

static void put16(uint8_t *rom, uint32_t pc, uint16_t value) {
    size_t offset = hirom_offset(pc);
    rom[offset] = (uint8_t)value;
    rom[offset + 1u] = (uint8_t)(value >> 8u);
}

static void map_cell(uint8_t *rom, uint32_t map_pc,
                     size_t column, size_t row, uint16_t block) {
    put16(rom, map_pc + (uint32_t)((column * DK1_TERRAIN_ROWS + row) * 2u), block);
}

int main(void) {
    const size_t rom_size = 4u * 1024u * 1024u;
    const uint32_t map_pc = 0xC00000u;
    const uint32_t collision_pc = 0xC10000u;
    uint8_t *bytes = (uint8_t *)calloc(rom_size, 1u);
    Dk1RomImage rom;
    Dk1LevelTerrainConfig config = {0};
    Dk1PlayerTerrainRuntime terrain;
    Dk1PlayerPreviewRuntime player;
    Dk1TerrainSample sample;
    uint16_t camera_y = 0u;
    unsigned i;

    assert(bytes != NULL);
    rom.data = bytes;
    rom.size = rom_size;
    config.map_snes = map_pc;
    config.collision_blocks_snes = collision_pc;
    config.block_count = 4u;

    /* Block 1: ground, block 2: wall material $40, block 3: ceiling $C0. */
    put16(bytes, collision_pc + 4u, 0x0001u);
    put16(bytes, collision_pc + 6u, 0x0001u);
    put16(bytes, collision_pc + 8u, 0x0041u);
    put16(bytes, collision_pc + 10u, 0x0041u);
    put16(bytes, collision_pc + 12u, 0x00C1u);
    put16(bytes, collision_pc + 14u, 0x00C1u);

    /* Ground at y=351, a wall beginning at x=64, and a ceiling above x=32..63. */
    for (i = 0u; i < 4u; ++i) map_cell(bytes, map_pc, i, 5u, 1u);
    map_cell(bytes, map_pc, 2u, 4u, 2u);
    map_cell(bytes, map_pc, 1u, 3u, 3u);

    assert(dk1_player_terrain_init(&terrain, &rom, config, 256u));
    assert(dk1_rom_terrain_point_solid(&terrain.view, 64, 367, &sample));
    assert(sample.attributes == 0x0040u);
    assert(!dk1_rom_terrain_point_solid(&terrain.view, 48, 367, &sample));

    dk1_player_preview_init(&player, 48u, 367);
    assert(dk1_player_terrain_place(&terrain, &player, 64u, &camera_y));
    assert(player.motion.world_y == 367);

    /* Move into the full-height cell at x=64: horizontal position is restored. */
    player.motion.velocity_x = 0x1000;
    player.motion.target_velocity_x = 0x1000;
    {
        uint16_t old_x = player.motion.world_x;
        int16_t old_y = player.motion.world_y;
        dk1_player_preview_integrate(&player);
        dk1_player_terrain_resolve_motion(&terrain, &player, old_x, old_y);
        assert(player.motion.world_x == old_x);
    }
    assert(terrain.blocked_right);
    assert(terrain.wall_contacts == 1u);
    assert(terrain.wall_attributes == 0x0040u);
    assert(player.motion.velocity_x == 0);

    /* Jump into the solid underside at y=384 and stop without becoming grounded. */
    player.motion.world_x = 48u;
    player.motion.world_y = 367;
    player.motion.velocity_y = 0x0500;
    player.airborne = true;
    for (i = 0u; i < 8u && !terrain.hit_ceiling; ++i) {
        uint16_t old_x = player.motion.world_x;
        int16_t old_y = player.motion.world_y;
        dk1_player_preview_integrate(&player);
        dk1_player_terrain_resolve_motion(&terrain, &player, old_x, old_y);
    }
    assert(terrain.hit_ceiling);
    assert(terrain.ceiling_contacts == 1u);
    assert(terrain.ceiling_attributes == 0x00C0u);
    assert(player.motion.velocity_y == 0);
    assert(player.airborne);

    free(bytes);
    return 0;
}
