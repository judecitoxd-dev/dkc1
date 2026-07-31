#include <assert.h>
#include "dk1/rgba_surface.h"

int main(void) {
    uint8_t indexed_pixels[4] = {0u, 1u, 1u, 0u};
    Dk1Rgba8 rgba_pixels[4];
    const Dk1Rgba8 palette[2] = {
        {1u, 2u, 3u, 255u},
        {4u, 5u, 6u, 255u},
    };
    Dk1IndexedSurface indexed = {indexed_pixels, 2u, 2u, 2u};
    Dk1RgbaSurface rgba = {rgba_pixels, 2u, 2u, 2u};
    const Dk1Rgba8 fill = {9u, 8u, 7u, 255u};
    assert(dk1_surface_indexed_to_rgba(rgba, indexed, palette, 2u));
    assert(rgba_pixels[1].r == 4u && rgba_pixels[1].g == 5u);
    dk1_surface_fill_rect(rgba, 1, 0, 1u, 2u, fill);
    assert(rgba_pixels[1].r == 9u && rgba_pixels[3].b == 7u);
    return 0;
}
