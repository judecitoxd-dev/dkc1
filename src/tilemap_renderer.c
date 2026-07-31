#include "dk1/tilemap_renderer.h"

static size_t wrap_coord(int64_t value, size_t extent) {
    int64_t result;
    if (extent == 0u) return 0u;
    result = value % (int64_t)extent;
    if (result < 0) result += (int64_t)extent;
    return (size_t)result;
}

bool dk1_tilemap_draw_wrapped(
    Dk1IndexedSurface surface,
    Dk1Tilemap map,
    Dk1TileAtlas atlas,
    uint16_t scroll_x,
    uint16_t scroll_y
) {
    size_t y = 0u;
    const size_t world_width = map.width_tiles * DK1_TILE_SIZE;
    const size_t world_height = map.height_tiles * DK1_TILE_SIZE;
    if (surface.pixels == 0 || surface.width == 0u || surface.height == 0u ||
        surface.stride < surface.width || map.tiles == 0 ||
        map.width_tiles == 0u || map.height_tiles == 0u ||
        atlas.pixels == 0 || atlas.tile_count == 0u) return false;

    for (y = 0u; y < surface.height; ++y) {
        size_t x = 0u;
        const size_t world_y = wrap_coord((int64_t)y + scroll_y, world_height);
        const size_t tile_y = world_y / DK1_TILE_SIZE;
        const size_t pixel_y = world_y % DK1_TILE_SIZE;
        for (x = 0u; x < surface.width; ++x) {
            const size_t world_x = wrap_coord((int64_t)x + scroll_x, world_width);
            const size_t tile_x = world_x / DK1_TILE_SIZE;
            const size_t pixel_x = world_x % DK1_TILE_SIZE;
            const uint16_t tile_id = map.tiles[tile_y * map.width_tiles + tile_x];
            const size_t safe_tile = (size_t)tile_id % atlas.tile_count;
            surface.pixels[y * surface.stride + x] =
                atlas.pixels[safe_tile * 64u + pixel_y * 8u + pixel_x];
        }
    }
    return true;
}
