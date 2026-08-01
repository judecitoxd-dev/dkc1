#include <stdio.h>
#include <stdlib.h>
#include "dk1/rom_image.h"
#include "dk1/scene_memory.h"
#include "dk1/scene_signature.h"

int main(int argc, char **argv) {
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom = {0};
    Dk1RomIdentity identity;
    Dk1SceneMemory *scene;
    size_t loaded = 0u;
    size_t failed = 0u;
    size_t texture_scenes = 0u;
    size_t palette_scenes = 0u;
    size_t package_records = 0u;
    size_t dma_bytes = 0u;
    size_t decompressed_bytes = 0u;
    uint64_t combined = 0u;
    int level;

    if (argc != 2) {
        fprintf(stderr, "usage: %s ROM\n", argv[0]);
        return 2;
    }
    if (!dk1_rom_load_file(argv[1], &owned, &rom) ||
        !dk1_rom_identity(&rom, &identity) ||
        !dk1_rom_identity_is_supported_rev2(&identity))
        return 1;

    scene = (Dk1SceneMemory *)malloc(sizeof(*scene));
    if (scene == NULL) {
        dk1_rom_free(&owned);
        return 1;
    }
    for (level = 0; level < DK1_SCENE_LEVEL_COUNT; ++level) {
        if (dk1_scene_memory_load(&rom, (uint16_t)level,
                                  false, false, scene)) {
            const uint64_t signature = dk1_scene_memory_signature(scene);
            combined ^= signature +
                (uint64_t)level * UINT64_C(0x9E3779B97F4A7C15);
            ++loaded;
            if (scene->assets.textures_loaded)
                ++texture_scenes;
            if (scene->assets.palettes_loaded)
                ++palette_scenes;
            package_records += scene->assets.package_records;
            dma_bytes += scene->assets.dma_bytes;
            decompressed_bytes += scene->assets.decompressed_bytes;
        } else {
            ++failed;
        }
    }
    printf("scenes=%zu failed=%zu textures_loaded=%zu palettes_loaded=%zu "
           "package_records=%zu dma_bytes=%zu decompressed_bytes=%zu "
           "signature=%016llX\n",
           loaded, failed, texture_scenes, palette_scenes,
           package_records, dma_bytes, decompressed_bytes,
           (unsigned long long)combined);
    free(scene);
    dk1_rom_free(&owned);
    return failed != 0u ? 1 : 0;
}
