#include <assert.h>
#include <string.h>
#include "dk1/tilemap_renderer.h"

int main(void) {
    uint8_t atlas_pixels[128];
    const uint16_t tiles[4] = {0u, 1u, 1u, 0u};
    uint8_t output[20u * 8u];
    Dk1TileAtlas atlas = {atlas_pixels, 2u};
    Dk1Tilemap map = {tiles, 2u, 2u};
    Dk1IndexedSurface surface = {output, 20u, 8u, 20u};
    memset(atlas_pixels, 3, 64u);
    memset(atlas_pixels + 64u, 9, 64u);
    memset(output, 0, sizeof(output));
    assert(dk1_tilemap_draw_wrapped(surface, map, atlas, 0u, 0u));
    assert(output[0] == 3u);
    assert(output[8] == 9u);
    assert(output[16] == 3u);
    assert(dk1_tilemap_draw_wrapped(surface, map, atlas, 8u, 0u));
    assert(output[0] == 9u);
    return 0;
}
