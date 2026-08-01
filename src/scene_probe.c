#include <stdio.h>
#include <stdlib.h>
#include "dk1/ppu_compositor.h"
#include "dk1/rom_image.h"
#include "dk1/scene_memory.h"
#include "dk1/scene_view.h"

int main(int argc, char **argv) {
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom = {0};
    Dk1RomIdentity identity;
    Dk1SceneMemory *scene = NULL;
    Dk1Rgba8 *pixels = NULL;
    Dk1Rgba8 *background = NULL;
    Dk1RgbaSurface output;
    Dk1RgbaSurface bg1;
    Dk1SceneView view;
    unsigned long level;
    unsigned long camera_x;
    unsigned long camera_y;
    unsigned long width;
    unsigned long height;
    bool streamed = false;
    int result = 1;

    if (argc != 8) {
        fprintf(stderr,
                "usage: %s ROM LEVEL X Y WIDTH HEIGHT OUTPUT.ppm\n",
                argv[0]);
        return 2;
    }
    level = strtoul(argv[2], NULL, 0);
    camera_x = strtoul(argv[3], NULL, 0);
    camera_y = strtoul(argv[4], NULL, 0);
    width = strtoul(argv[5], NULL, 0);
    height = strtoul(argv[6], NULL, 0);
    if (level >= DK1_SCENE_LEVEL_COUNT || width == 0u || height == 0u ||
        width > 1024u || height > 512u)
        return 2;

    if (!dk1_rom_load_file(argv[1], &owned, &rom) ||
        !dk1_rom_identity(&rom, &identity) ||
        !dk1_rom_identity_is_supported_rev2(&identity))
        goto cleanup;

    scene = (Dk1SceneMemory *)malloc(sizeof(*scene));
    pixels = (Dk1Rgba8 *)calloc(width * height, sizeof(*pixels));
    background = (Dk1Rgba8 *)calloc(width * height, sizeof(*background));
    if (scene == NULL || pixels == NULL || background == NULL ||
        !dk1_scene_memory_load(&rom, (uint16_t)level,
                               false, false, scene))
        goto cleanup;

    output = (Dk1RgbaSurface){pixels, width, height, width};
    bg1 = (Dk1RgbaSurface){background, width, height, width};
    view = (Dk1SceneView){(uint16_t)camera_x, (uint16_t)camera_y,
                          (uint16_t)width, (uint16_t)height};
    if (!dk1_scene_view_clamp(scene, &view) ||
        !dk1_ppu_compose_mode1(scene,
                               view.camera_x / 2u,
                               view.camera_y / 2u,
                               0u, view.camera_y, output))
        goto cleanup;

    streamed = dk1_scene_render_bg1(&rom, scene, view, bg1);
    if (streamed) {
        size_t i;
        for (i = 0u; i < width * height; ++i)
            output.pixels[i] = bg1.pixels[i];
    }
    if (!dk1_surface_write_ppm(argv[7], output))
        goto cleanup;

    printf("level=%lu callback=$%04X ppu=%u terrain=%u package=%u "
           "camera=%u,%u bounds=%u..%u source=%s\n",
           level,
           scene->recipe.init_callback_b9,
           scene->recipe.ppu_preset,
           scene->recipe.terrain_profile,
           scene->recipe.primary_vram_package,
           view.camera_x, view.camera_y,
           scene->camera.minimum_x, scene->camera.maximum_x,
           streamed ? "stream-map" : "vram");
    printf("rom-assets textures=%s palettes=%s packages=%zu records=%zu "
           "direct=%zu compressed=%zu dma=%zu decompressed=%zu "
           "vram_nonzero=%zu palette_uploads=%zu palette_colors=%zu "
           "palette_nonzero=%zu vram_sig=%016llX palette_sig=%016llX\n",
           scene->assets.textures_loaded ? "loaded" : "empty",
           scene->assets.palettes_loaded ? "loaded" : "empty",
           scene->assets.package_count,
           scene->assets.package_records,
           scene->assets.direct_records,
           scene->assets.compressed_records,
           scene->assets.dma_bytes,
           scene->assets.decompressed_bytes,
           scene->assets.nonzero_vram_bytes,
           scene->assets.palette_uploads,
           scene->assets.palette_colors,
           scene->assets.nonzero_palette_colors,
           (unsigned long long)scene->assets.vram_signature,
           (unsigned long long)scene->assets.palette_signature);
    result = 0;

cleanup:
    free(background);
    free(pixels);
    free(scene);
    dk1_rom_free(&owned);
    return result;
}
