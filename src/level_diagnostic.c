#include "dk1/level_diagnostic.h"

#include <stdlib.h>

static Dk1Rgba8 tile_color(uint16_t word) {
    const uint16_t tile = (uint16_t)(word & 0x03FFu);
    const uint8_t palette = (uint8_t)((word >> 10u) & 7u);
    Dk1Rgba8 color;
    color.r = (uint8_t)((tile * 37u + palette * 19u) & 0xFFu);
    color.g = (uint8_t)((tile * 73u + palette * 31u) & 0xFFu);
    color.b = (uint8_t)((tile * 17u + palette * 47u) & 0xFFu);
    color.a = 255u;
    if ((word & 0x4000u) != 0u) color.r = (uint8_t)(color.r / 2u + 64u);
    if ((word & 0x8000u) != 0u) color.b = (uint8_t)(color.b / 2u + 64u);
    return color;
}

bool dk1_level_diagnostic_render(const Dk1RomImage *rom,
                                 Dk1LevelTerrainConfig config,
                                 size_t first_column, size_t column_count,
                                 Dk1RgbaSurface surface) {
    const size_t tile_width = column_count * 4u;
    uint16_t *tiles = NULL;
    Dk1RomTerrainView terrain;
    size_t y = 0u;
    if (rom == NULL || surface.pixels == NULL || column_count == 0u ||
        surface.width < column_count * 32u || surface.height < 512u ||
        surface.stride < surface.width) return false;
    tiles = (uint16_t *)calloc(tile_width * 64u, sizeof(*tiles));
    if (tiles == NULL) return false;
    if (!dk1_level_map_expand_tiles(rom, &config, first_column, column_count,
                                    tiles, tile_width, 64u)) {
        free(tiles);
        return false;
    }
    for (y = 0u; y < 64u; ++y) {
        size_t x = 0u;
        for (x = 0u; x < tile_width; ++x) {
            dk1_surface_fill_rect(surface, (int32_t)(x * 8u), (int32_t)(y * 8u),
                                  8u, 8u, tile_color(tiles[y * tile_width + x]));
        }
    }
    free(tiles);
    if (!dk1_rom_terrain_view_init(&terrain, rom, config,
                                   first_column + column_count, NULL)) return false;
    {
        size_t px = 0u;
        const Dk1Rgba8 floor_color = {255u, 32u, 32u, 255u};
        for (px = 0u; px < column_count * 32u; ++px) {
            Dk1TerrainSample sample;
            const int32_t world_x = (int32_t)(first_column * 32u + px);
            if (dk1_rom_terrain_find_floor(&terrain, world_x, 0, 15u, &sample)) {
                dk1_surface_fill_rect(surface, (int32_t)px, sample.floor_y,
                                      1u, 2u, floor_color);
            }
        }
    }
    return true;
}
