#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dk1/rgba_surface.h"
#include "dk1/rom_image.h"
#include "dk1/snes_graphics.h"
#include "dk1/tilemap_renderer.h"

static bool parse_pc(const char *text, uint32_t *pc) {
    char *end = NULL;
    unsigned long value = 0ul;
    if (text == NULL || pc == NULL) return false;
    errno = 0;
    value = strtoul(text, &end, 16);
    if (errno != 0 || end == text || *end != '\0' || value > 0xFFFFFFul) return false;
    *pc = (uint32_t)value;
    return true;
}

int main(int argc, char **argv) {
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom = {0};
    Dk1RomIdentity identity;
    uint32_t tile_pc = 0u;
    uint32_t palette_pc = 0u;
    uint8_t tile_pixels[16u * 64u];
    uint16_t map_tiles[16u];
    uint8_t indexed[32u * 32u];
    Dk1Rgba8 palette[16u];
    Dk1Rgba8 rgba[32u * 32u];
    Dk1IndexedSurface indexed_surface = {indexed, 32u, 32u, 32u};
    Dk1RgbaSurface rgba_surface = {rgba, 32u, 32u, 32u};
    Dk1TileAtlas atlas = {tile_pixels, 16u};
    Dk1Tilemap map = {map_tiles, 4u, 4u};
    size_t i = 0u;

    if (argc != 5 || !parse_pc(argv[2], &tile_pc) || !parse_pc(argv[3], &palette_pc)) {
        fprintf(stderr, "usage: %s ROM TILE_PC_HEX PALETTE_PC_HEX output.ppm\n", argv[0]);
        return 2;
    }
    if (!dk1_rom_load_file(argv[1], &owned, &rom) ||
        !dk1_rom_identity(&rom, &identity) ||
        !dk1_rom_identity_is_supported_rev2(&identity)) {
        fprintf(stderr, "unsupported or unreadable ROM\n");
        dk1_rom_free(&owned);
        return 3;
    }
    if (!dk1_rom_decode_4bpp_tiles(&rom, tile_pc, tile_pixels, 16u) ||
        !dk1_rom_decode_palette(&rom, palette_pc, palette, 16u, false)) {
        fprintf(stderr, "asset range is outside ROM or not readable\n");
        dk1_rom_free(&owned);
        return 4;
    }
    for (i = 0u; i < 16u; ++i) map_tiles[i] = (uint16_t)i;
    if (!dk1_tilemap_draw_wrapped(indexed_surface, map, atlas, 0u, 0u) ||
        !dk1_surface_indexed_to_rgba(rgba_surface, indexed_surface, palette, 16u) ||
        !dk1_surface_write_ppm(argv[4], rgba_surface)) {
        fprintf(stderr, "failed to render output\n");
        dk1_rom_free(&owned);
        return 5;
    }
    dk1_rom_free(&owned);
    printf("wrote %s from tiles $%06X and palette $%06X\n",
           argv[4], (unsigned)tile_pc, (unsigned)palette_pc);
    return 0;
}
