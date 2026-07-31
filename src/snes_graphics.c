#include "dk1/snes_graphics.h"

bool dk1_snes_decode_4bpp_tile(const uint8_t source[DK1_SNES_4BPP_TILE_BYTES], uint8_t output[DK1_SNES_TILE_PIXELS]) {
    size_t y = 0u;
    if (source == NULL || output == NULL) return false;
    for (y = 0u; y < 8u; ++y) {
        const uint8_t p0 = source[y * 2u];
        const uint8_t p1 = source[y * 2u + 1u];
        const uint8_t p2 = source[16u + y * 2u];
        const uint8_t p3 = source[16u + y * 2u + 1u];
        size_t x = 0u;
        for (x = 0u; x < 8u; ++x) {
            const unsigned bit = (unsigned)(7u - x);
            output[y * 8u + x] = (uint8_t)(((p0 >> bit) & 1u) | (((p1 >> bit) & 1u) << 1u) | (((p2 >> bit) & 1u) << 2u) | (((p3 >> bit) & 1u) << 3u));
        }
    }
    return true;
}

bool dk1_snes_decode_4bpp_tiles(const uint8_t *source, size_t source_size, uint8_t *output, size_t tile_count) {
    size_t i = 0u;
    if (source == NULL || output == NULL || tile_count > source_size / DK1_SNES_4BPP_TILE_BYTES) return false;
    for (i = 0u; i < tile_count; ++i) {
        if (!dk1_snes_decode_4bpp_tile(source + i * DK1_SNES_4BPP_TILE_BYTES, output + i * DK1_SNES_TILE_PIXELS)) return false;
    }
    return true;
}

static uint8_t expand5(uint16_t value) {
    value &= 0x1Fu;
    return (uint8_t)((value << 3u) | (value >> 2u));
}

Dk1Rgba8 dk1_snes_bgr555_to_rgba(uint16_t color, bool transparent_zero) {
    Dk1Rgba8 result;
    result.r = expand5(color);
    result.g = expand5(color >> 5u);
    result.b = expand5(color >> 10u);
    result.a = transparent_zero && (color & 0x7FFFu) == 0u ? 0u : 255u;
    return result;
}

bool dk1_snes_decode_palette(const uint8_t *source, size_t source_size, Dk1Rgba8 *output, size_t color_count, bool first_color_transparent) {
    size_t i = 0u;
    if (source == NULL || output == NULL || color_count > source_size / 2u) return false;
    for (i = 0u; i < color_count; ++i) {
        const uint16_t color = (uint16_t)(source[i * 2u] | ((uint16_t)source[i * 2u + 1u] << 8u));
        output[i] = dk1_snes_bgr555_to_rgba(color, first_color_transparent && i == 0u);
    }
    return true;
}

bool dk1_rom_decode_4bpp_tiles(const Dk1RomImage *rom, uint32_t source_pc, uint8_t *output, size_t tile_count) {
    size_t offset = 0u;
    size_t byte_count = 0u;
    if (rom == NULL || output == NULL || tile_count > SIZE_MAX / DK1_SNES_4BPP_TILE_BYTES) return false;
    byte_count = tile_count * DK1_SNES_4BPP_TILE_BYTES;
    if (!dk1_hirom_offset(source_pc, rom->size, &offset) || byte_count > rom->size - offset) return false;
    return dk1_snes_decode_4bpp_tiles(rom->data + offset, byte_count, output, tile_count);
}

bool dk1_rom_decode_palette(const Dk1RomImage *rom, uint32_t source_pc, Dk1Rgba8 *output, size_t color_count, bool first_color_transparent) {
    size_t offset = 0u;
    size_t byte_count = 0u;
    if (rom == NULL || output == NULL || color_count > SIZE_MAX / 2u) return false;
    byte_count = color_count * 2u;
    if (!dk1_hirom_offset(source_pc, rom->size, &offset) || byte_count > rom->size - offset) return false;
    return dk1_snes_decode_palette(rom->data + offset, byte_count, output, color_count, first_color_transparent);
}
