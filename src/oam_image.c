#include "dk1/oam_image.h"
#include <string.h>

static void size_pair(uint8_t mode, bool large, uint8_t *width, uint8_t *height) {
    static const uint8_t sizes[8][4] = {
        {8,8,16,16}, {8,8,32,32}, {8,8,64,64}, {16,16,32,32},
        {16,16,64,64}, {32,32,64,64}, {16,32,32,64}, {16,32,32,32}
    };
    const uint8_t *entry = sizes[mode & 7u];
    *width = entry[large ? 2u : 0u];
    *height = entry[large ? 3u : 1u];
}

void dk1_oam_init(Dk1OamImage *oam) {
    size_t i;
    if (oam == NULL) return;
    memset(oam, 0, sizeof(*oam));
    for (i = 0; i < DK1_OAM_SPRITE_COUNT; ++i) oam->low[i * 4u + 1u] = 0xF0u;
}

uint16_t dk1_oam_character_base_word(uint8_t obsel, bool name_select) {
    uint16_t base = (uint16_t)(obsel & 7u) * 0x1000u;
    if (name_select) base = (uint16_t)(base + ((uint16_t)(((obsel >> 3u) & 3u) + 1u) * 0x0800u));
    return (uint16_t)(base & 0x7FFFu);
}

bool dk1_oam_decode_sprite(const Dk1OamImage *oam, uint8_t obsel,
                           size_t sprite_index, Dk1OamSprite *sprite) {
    size_t low_index, high_index, shift;
    uint8_t packed, attr;
    bool x_high;
    if (oam == NULL || sprite == NULL || sprite_index >= DK1_OAM_SPRITE_COUNT) return false;
    low_index = sprite_index * 4u;
    high_index = sprite_index / 4u;
    shift = (sprite_index % 4u) * 2u;
    packed = (uint8_t)((oam->high[high_index] >> shift) & 3u);
    attr = oam->low[low_index + 3u];
    x_high = (packed & 1u) != 0u;
    sprite->x = (int16_t)((uint16_t)oam->low[low_index] | (x_high ? 0x100u : 0u));
    if (sprite->x >= 256) sprite->x = (int16_t)(sprite->x - 512);
    sprite->y = oam->low[low_index + 1u];
    sprite->tile_number = (uint16_t)oam->low[low_index + 2u] | (uint16_t)((attr & 1u) << 8u);
    sprite->palette = (uint8_t)((attr >> 1u) & 7u);
    sprite->priority = (uint8_t)((attr >> 4u) & 3u);
    sprite->horizontal_flip = (attr & 0x40u) != 0u;
    sprite->vertical_flip = (attr & 0x80u) != 0u;
    sprite->large = (packed & 2u) != 0u;
    size_pair((uint8_t)(obsel >> 5u), sprite->large, &sprite->width, &sprite->height);
    return true;
}

bool dk1_oam_encode_sprite(Dk1OamImage *oam, uint8_t obsel,
                           size_t sprite_index, const Dk1OamSprite *sprite) {
    size_t low_index, high_index, shift;
    uint16_t encoded_x;
    uint8_t packed, expected_w, expected_h, attr;
    bool large;
    if (oam == NULL || sprite == NULL || sprite_index >= DK1_OAM_SPRITE_COUNT ||
        sprite->x < -256 || sprite->x > 255 || sprite->palette > 7u || sprite->priority > 3u ||
        sprite->tile_number > 0x1FFu) return false;
    size_pair((uint8_t)(obsel >> 5u), false, &expected_w, &expected_h);
    if (sprite->width == expected_w && sprite->height == expected_h) large = false;
    else {
        size_pair((uint8_t)(obsel >> 5u), true, &expected_w, &expected_h);
        if (sprite->width != expected_w || sprite->height != expected_h) return false;
        large = true;
    }
    encoded_x = (uint16_t)(sprite->x < 0 ? sprite->x + 512 : sprite->x);
    low_index = sprite_index * 4u;
    high_index = sprite_index / 4u;
    shift = (sprite_index % 4u) * 2u;
    oam->low[low_index] = (uint8_t)encoded_x;
    oam->low[low_index + 1u] = sprite->y;
    oam->low[low_index + 2u] = (uint8_t)sprite->tile_number;
    attr = (uint8_t)((sprite->tile_number >> 8u) & 1u);
    attr |= (uint8_t)(sprite->palette << 1u);
    attr |= (uint8_t)(sprite->priority << 4u);
    if (sprite->horizontal_flip) attr |= 0x40u;
    if (sprite->vertical_flip) attr |= 0x80u;
    oam->low[low_index + 3u] = attr;
    packed = (uint8_t)(((encoded_x >> 8u) & 1u) | (large ? 2u : 0u));
    oam->high[high_index] = (uint8_t)((oam->high[high_index] & ~(3u << shift)) | (packed << shift));
    return true;
}

static bool read_tile(const Dk1VramImage *vram, uint16_t base_word, uint16_t tile,
                      uint8_t pixels[64]) {
    uint8_t raw[32];
    size_t i, base = ((size_t)base_word * 2u + (size_t)(tile & 0xFFu) * 32u) & 0xFFFFu;
    for (i = 0; i < sizeof(raw); ++i) raw[i] = vram->bytes[(base + i) & 0xFFFFu];
    return dk1_snes_decode_4bpp_tile(raw, pixels);
}

bool dk1_oam_render(const Dk1OamImage *oam, uint8_t obsel,
                    const Dk1VramImage *vram, const Dk1CgramImage *cgram,
                    uint8_t minimum_priority, uint8_t maximum_priority,
                    Dk1RgbaSurface destination) {
    Dk1Rgba8 palette[DK1_CGRAM_COLOR_COUNT];
    int sprite_index;
    if (oam == NULL || vram == NULL || cgram == NULL || destination.pixels == NULL ||
        destination.stride < destination.width || minimum_priority > maximum_priority || maximum_priority > 3u) return false;
    dk1_cgram_decode_rgba(cgram, palette);
    for (sprite_index = (int)DK1_OAM_SPRITE_COUNT - 1; sprite_index >= 0; --sprite_index) {
        Dk1OamSprite sprite;
        size_t tile_y, tile_x;
        if (!dk1_oam_decode_sprite(oam, obsel, (size_t)sprite_index, &sprite)) return false;
        if (sprite.priority < minimum_priority || sprite.priority > maximum_priority) continue;
        if (sprite.y >= 224u && sprite.y < (uint8_t)(256u - sprite.height)) continue;
        for (tile_y = 0; tile_y < (size_t)sprite.height / 8u; ++tile_y) {
            for (tile_x = 0; tile_x < (size_t)sprite.width / 8u; ++tile_x) {
                size_t logical_x = sprite.horizontal_flip ? ((size_t)sprite.width / 8u - 1u - tile_x) : tile_x;
                size_t logical_y = sprite.vertical_flip ? ((size_t)sprite.height / 8u - 1u - tile_y) : tile_y;
                uint16_t tile = (uint16_t)((sprite.tile_number & 0x100u) |
                    ((sprite.tile_number + logical_x + logical_y * 16u) & 0xFFu));
                uint16_t base = dk1_oam_character_base_word(obsel, (tile & 0x100u) != 0u);
                uint8_t pixels[64];
                size_t py, px;
                if (!read_tile(vram, base, tile, pixels)) return false;
                for (py = 0; py < 8u; ++py) for (px = 0; px < 8u; ++px) {
                    size_t source_x = sprite.horizontal_flip ? 7u - px : px;
                    size_t source_y = sprite.vertical_flip ? 7u - py : py;
                    uint8_t value = pixels[source_y * 8u + source_x];
                    int32_t dx = (int32_t)sprite.x + (int32_t)(tile_x * 8u + px);
                    int32_t dy = (int32_t)sprite.y + (int32_t)(tile_y * 8u + py);
                    if (value == 0u || dx < 0 || dy < 0 || (size_t)dx >= destination.width || (size_t)dy >= destination.height) continue;
                    destination.pixels[(size_t)dy * destination.stride + (size_t)dx] =
                        palette[128u + (size_t)sprite.palette * 16u + value];
                }
            }
        }
    }
    return true;
}
