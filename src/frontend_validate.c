#include <stdio.h>
#include <stdlib.h>
#include "dk1/level_software_frontend.h"
#include "dk1/rom_image.h"
#include "dk1/scene_memory.h"
#include "dk1/scene_signature.h"

int main(int argc, char **argv) {
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom = {0};
    Dk1RomIdentity identity;
    Dk1SceneMemory *scene = NULL;
    Dk1Rgba8 *pixels = NULL;
    Dk1RgbaSurface surface;
    uint64_t hash = 0u;
    uint16_t level;
    size_t failed = 0u;
    size_t terrain = 0u;
    size_t local = 0u;
    size_t barrel_records = 0u;
    size_t barrel_spawns = 0u;

    if (argc != 2) return 2;
    if (!dk1_rom_load_file(argv[1], &owned, &rom) ||
        !dk1_rom_identity(&rom, &identity) ||
        !dk1_rom_identity_is_supported_rev2(&identity))
        return 1;

    scene = (Dk1SceneMemory *)malloc(sizeof(*scene));
    pixels = (Dk1Rgba8 *)calloc(96u * 64u, sizeof(*pixels));
    if (scene == NULL || pixels == NULL) {
        free(scene);
        free(pixels);
        dk1_rom_free(&owned);
        return 1;
    }
    surface = (Dk1RgbaSurface){pixels, 96u, 64u, 96u};

    for (level = 0u; level < DK1_SCENE_LEVEL_COUNT; ++level) {
        Dk1SoftwareFrontend frontend;
        Dk1LevelSoftwareFrontendStats source_stats;
        if (!dk1_scene_memory_load(&rom, level, false, false, scene) ||
            !dk1_level_software_frontend_init(&rom, scene, level,
                                              96u, 64u,
                                              &frontend, &source_stats) ||
            !dk1_software_frontend_step(scene, DK1_HOST_BUTTON_B,
                                        &frontend) ||
            !dk1_software_frontend_render(&rom, scene, &frontend, surface)) {
            ++failed;
            continue;
        }
        if (frontend.player_terrain_ready) ++terrain;
        if (frontend.player_live.used_local_semantics) ++local;
        if (source_stats.barrel_found) ++barrel_records;
        if (source_stats.barrel_spawned) ++barrel_spawns;
        hash = dk1_fnv1a64(&level, sizeof(level), hash);
        hash = dk1_fnv1a64(&frontend.runtime, sizeof(frontend.runtime), hash);
        hash = dk1_fnv1a64(&frontend.player_preview,
                           sizeof(frontend.player_preview), hash);
        hash = dk1_fnv1a64(&frontend.player_live,
                           sizeof(frontend.player_live), hash);
        hash = dk1_fnv1a64(&source_stats.barrel_found,
                           sizeof(source_stats.barrel_found), hash);
        if (source_stats.barrel_found) {
            hash = dk1_fnv1a64(&source_stats.barrel_spawn.record,
                               sizeof(source_stats.barrel_spawn.record), hash);
            hash = dk1_fnv1a64(&source_stats.barrel_spawn.callback_pc,
                               sizeof(source_stats.barrel_spawn.callback_pc),
                               hash);
        }
        hash = dk1_fnv1a64(pixels, 96u * 64u * sizeof(*pixels), hash);
    }

    printf("frontends=%u failed=%zu terrain=%zu local_dispatch=%zu "
           "barrel_records=%zu barrel_spawns=%zu signature=%016llX\n",
           DK1_SCENE_LEVEL_COUNT, failed, terrain, local,
           barrel_records, barrel_spawns, (unsigned long long)hash);
    free(scene);
    free(pixels);
    dk1_rom_free(&owned);
    return failed != 0u ? 1 : 0;
}
