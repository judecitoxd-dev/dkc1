#include <assert.h>
#include <string.h>
#include "dk1/oam_image.h"

int main(void) {
    Dk1OamImage oam;
    Dk1OamSprite in = {-12, 30, 0x123u, 5u, 2u, true, false, true, 32u, 32u};
    Dk1OamSprite out;
    Dk1VramImage vram;
    Dk1CgramImage cgram;
    Dk1Rgba8 pixels[64 * 64];
    Dk1RgbaSurface surface = {pixels, 64u, 64u, 64u};
    size_t i;
    dk1_oam_init(&oam);
    assert(dk1_oam_encode_sprite(&oam, (uint8_t)(3u << 5u), 7u, &in));
    assert(dk1_oam_decode_sprite(&oam, (uint8_t)(3u << 5u), 7u, &out));
    assert(out.x == -12 && out.y == 30u && out.tile_number == 0x123u);
    assert(out.palette == 5u && out.priority == 2u && out.horizontal_flip && !out.vertical_flip);
    assert(out.width == 32u && out.height == 32u && out.large);
    assert(dk1_oam_character_base_word(0u, false) == 0u);
    assert(dk1_oam_character_base_word(0u, true) == 0x0800u);

    dk1_vram_image_init(&vram);
    dk1_cgram_init(&cgram);
    cgram.colors[128u + 1u] = 0x001Fu;
    memset(vram.bytes, 0, sizeof(vram.bytes));
    for (i = 0; i < 8u; ++i) vram.bytes[i * 2u] = 0xFFu;
    dk1_oam_init(&oam);
    in.x = 4; in.y = 5; in.tile_number = 0; in.palette = 0; in.priority = 1;
    in.horizontal_flip = false; in.vertical_flip = false; in.large = false; in.width = 8; in.height = 8;
    assert(dk1_oam_encode_sprite(&oam, 0u, 0u, &in));
    memset(pixels, 0, sizeof(pixels));
    assert(dk1_oam_render(&oam, 0u, &vram, &cgram, 0u, 3u, surface));
    assert(pixels[5u * 64u + 4u].r == 255u);
    assert(pixels[5u * 64u + 4u].a == 255u);
    return 0;
}
