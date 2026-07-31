#include <assert.h>
#include <string.h>
#include "dk1/snes_graphics.h"

int main(void) {
    uint8_t source[32] = {0};
    uint8_t pixels[64] = {0};
    const uint8_t palette_bytes[4] = {0x1Fu, 0x00u, 0x00u, 0x7Cu};
    Dk1Rgba8 palette[2];

    source[0] = 0x80u;
    source[1] = 0x81u;
    source[16] = 0x80u;
    source[17] = 0x81u;
    assert(dk1_snes_decode_4bpp_tile(source, pixels));
    assert(pixels[0] == 15u);
    assert(pixels[7] == 10u);
    assert(pixels[8] == 0u);

    assert(dk1_snes_decode_palette(palette_bytes, sizeof(palette_bytes),
                                   palette, 2u, false));
    assert(palette[0].r == 255u && palette[0].g == 0u && palette[0].b == 0u);
    assert(palette[1].r == 0u && palette[1].g == 0u && palette[1].b == 255u);
    assert(dk1_snes_bgr555_to_rgba(0u, true).a == 0u);
    return 0;
}
