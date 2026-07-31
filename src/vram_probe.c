#include <stdio.h>
#include <stdlib.h>
#include "dk1/rgba_surface.h"
#include "dk1/rom_image.h"
#include "dk1/snes_graphics.h"
#include "dk1/vram_image.h"

static unsigned long parse_number(const char *text) {
    return strtoul(text, NULL, 0);
}

int main(int argc, char **argv) {
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom = {0};
    Dk1RomIdentity identity;
    Dk1VramImage *vram = NULL;
    Dk1VramLoadStats stats;
    Dk1Rgba8 palette[128];
    Dk1Rgba8 *pixels = NULL;
    Dk1RgbaSurface surface;
    unsigned long package_id = 0u;
    unsigned long palette_snes = 0u;
    unsigned long tile_base_word = 0u;
    unsigned long palette_id = 0u;
    unsigned long tile_count = 0u;
    size_t columns = 16u;
    size_t rows = 0u;
    size_t tile = 0u;
    int result = 1;

    if (argc != 8) {
        fprintf(stderr,
            "usage: %s ROM PACKAGE_ID PALETTE_PC TILE_BASE_WORD PALETTE_ID TILE_COUNT OUTPUT.ppm\n",
            argv[0]);
        return 2;
    }
    package_id = parse_number(argv[2]);
    palette_snes = parse_number(argv[3]);
    tile_base_word = parse_number(argv[4]);
    palette_id = parse_number(argv[5]);
    tile_count = parse_number(argv[6]);
    if (package_id >= 0x100u || palette_snes > 0xFFFFFFu ||
        tile_base_word > 0x7FFFu || palette_id >= 8u ||
        tile_count == 0u || tile_count > 1024u) {
        fputs("invalid numeric argument\n", stderr);
        return 2;
    }
    if (!dk1_rom_load_file(argv[1], &owned, &rom) ||
        !dk1_rom_identity(&rom, &identity) ||
        !dk1_rom_identity_is_supported_rev2(&identity)) {
        fputs("unsupported ROM\n", stderr);
        goto cleanup;
    }
    vram = (Dk1VramImage *)malloc(sizeof(*vram));
    rows = ((size_t)tile_count + columns - 1u) / columns;
    pixels = (Dk1Rgba8 *)calloc(columns * 8u * rows * 8u, sizeof(*pixels));
    if (vram == NULL || pixels == NULL ||
        !dk1_rom_decode_palette(&rom, (uint32_t)palette_snes,
                                palette, 128u, false)) goto cleanup;
    dk1_vram_image_init(vram);
    if (!dk1_vram_apply_package(&rom, (uint16_t)package_id, vram, &stats)) {
        fputs("could not apply VRAM package\n", stderr);
        goto cleanup;
    }
    surface.pixels = pixels;
    surface.width = columns * 8u;
    surface.height = rows * 8u;
    surface.stride = surface.width;
    for (tile = 0u; tile < tile_count; ++tile) {
        uint8_t indices[DK1_SNES_TILE_PIXELS];
        const size_t source = (size_t)tile_base_word * 2u +
                              tile * DK1_SNES_4BPP_TILE_BYTES;
        size_t y = 0u;
        if (source + DK1_SNES_4BPP_TILE_BYTES > DK1_VRAM_BYTE_COUNT ||
            !dk1_snes_decode_4bpp_tile(vram->bytes + source, indices)) {
            goto cleanup;
        }
        for (y = 0u; y < 8u; ++y) {
            size_t x = 0u;
            for (x = 0u; x < 8u; ++x) {
                const size_t destination_x = (tile % columns) * 8u + x;
                const size_t destination_y = (tile / columns) * 8u + y;
                const uint8_t index = indices[y * 8u + x];
                surface.pixels[destination_y * surface.stride + destination_x] =
                    palette[palette_id * 16u + index];
            }
        }
    }
    if (!dk1_surface_write_ppm(argv[7], surface)) goto cleanup;
    printf("package=%lu records=%u compressed=%u direct=%u dma=%u decompressed=%u tiles=%lu\n",
           package_id, (unsigned)stats.records,
           (unsigned)stats.compressed_records, (unsigned)stats.direct_records,
           (unsigned)stats.dma_bytes, (unsigned)stats.decompressed_bytes,
           tile_count);
    result = 0;
cleanup:
    free(pixels);
    free(vram);
    dk1_rom_free(&owned);
    return result;
}
