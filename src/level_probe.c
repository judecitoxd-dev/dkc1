#include <stdio.h>
#include <stdlib.h>
#include "dk1/level_diagnostic.h"
#include "dk1/level_terrain_config.h"
#include "dk1/rom_image.h"

int main(int argc, char **argv) {
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom = {0};
    Dk1RomIdentity identity;
    Dk1LevelTerrainConfig config;
    Dk1Rgba8 *pixels = NULL;
    Dk1RgbaSurface surface;
    unsigned long config_id = 0u;
    unsigned long columns = 0u;
    int result = 1;
    if (argc != 5) {
        fprintf(stderr, "usage: %s ROM CONFIG_ID COLUMNS OUTPUT.ppm\n", argv[0]);
        return 2;
    }
    config_id = strtoul(argv[2], NULL, 0);
    columns = strtoul(argv[3], NULL, 0);
    if (config_id >= DK1_LEVEL_TERRAIN_CONFIG_COUNT || columns == 0u || columns > 128u) {
        fputs("invalid config id or column count\n", stderr);
        return 2;
    }
    if (!dk1_rom_load_file(argv[1], &owned, &rom) ||
        !dk1_rom_identity(&rom, &identity) ||
        !dk1_rom_identity_is_supported_rev2(&identity) ||
        !dk1_level_terrain_config_read(&rom, (uint16_t)config_id, &config)) {
        fputs("unsupported ROM or unreadable level config\n", stderr);
        goto cleanup;
    }
    surface.width = (size_t)columns * 32u;
    surface.height = 512u;
    surface.stride = surface.width;
    pixels = (Dk1Rgba8 *)calloc(surface.width * surface.height, sizeof(*pixels));
    if (pixels == NULL) goto cleanup;
    surface.pixels = pixels;
    if (!dk1_level_diagnostic_render(&rom, config, 0u, (size_t)columns, surface) ||
        !dk1_surface_write_ppm(argv[4], surface)) {
        fputs("could not render diagnostic level map\n", stderr);
        goto cleanup;
    }
    printf("config=%lu map=$%06X visual=$%02X:%04X collision=$%06X blocks=%u callback=$%06X\n",
           config_id, (unsigned)config.map_snes, (unsigned)config.visual_bank,
           (unsigned)config.visual_block_base,
           (unsigned)config.collision_blocks_snes, (unsigned)config.block_count,
           (unsigned)config.terrain_callback_snes);
    result = 0;
cleanup:
    free(pixels);
    dk1_rom_free(&owned);
    return result;
}
