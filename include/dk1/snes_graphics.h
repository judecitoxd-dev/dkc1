#ifndef DK1_SNES_GRAPHICS_H
#define DK1_SNES_GRAPHICS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/rom_image.h"

#define DK1_SNES_4BPP_TILE_BYTES 32u
#define DK1_SNES_TILE_PIXELS 64u

typedef struct Dk1Rgba8 {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} Dk1Rgba8;

bool dk1_snes_decode_4bpp_tile(const uint8_t source[DK1_SNES_4BPP_TILE_BYTES], uint8_t output[DK1_SNES_TILE_PIXELS]);
bool dk1_snes_decode_4bpp_tiles(const uint8_t *source, size_t source_size, uint8_t *output, size_t tile_count);
Dk1Rgba8 dk1_snes_bgr555_to_rgba(uint16_t color, bool transparent_zero);
bool dk1_snes_decode_palette(const uint8_t *source, size_t source_size, Dk1Rgba8 *output, size_t color_count, bool first_color_transparent);
bool dk1_rom_decode_4bpp_tiles(const Dk1RomImage *rom, uint32_t source_pc, uint8_t *output, size_t tile_count);
bool dk1_rom_decode_palette(const Dk1RomImage *rom, uint32_t source_pc, Dk1Rgba8 *output, size_t color_count, bool first_color_transparent);

#endif
