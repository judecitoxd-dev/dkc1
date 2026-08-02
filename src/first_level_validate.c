#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dk1/first_level_route.h"
#include "dk1/level_software_frontend.h"
#include "dk1/preview_asset_warmup.h"
#include "dk1/rom_image.h"
#include "dk1/scene_asset_cache.h"
#include "dk1/scene_memory.h"
#include "dk1/scene_signature.h"
#include "dk1/scene_view.h"

#define DK1_FIRST_LEVEL_ID 0x0016u
#define DK1_FIRST_LEVEL_WIDTH 384u
#define DK1_FIRST_LEVEL_HEIGHT 224u
#define DK1_FIRST_LEVEL_CHECKPOINT_COUNT 6u
#define DK1_FIRST_LEVEL_SEARCH_RADIUS 96u
#define DK1_FIRST_LEVEL_SEARCH_STEP 8u

static const uint8_t checkpoint_percent[DK1_FIRST_LEVEL_CHECKPOINT_COUNT] = {
    0u, 20u, 40u, 60u, 80u, 95u
};

typedef struct Dk1FirstLevelCheckpointResult {
    uint8_t requested_percent;
    uint16_t world_x;
    int16_t world_y;
    uint16_t camera_x;
    uint16_t camera_y;
    size_t active_objects;
    size_t active_gnawties;
    size_t active_barrels;
    size_t nonzero_pixels;
    uint32_t bg1_generation;
    size_t bg1_columns;
    uint64_t frame_signature;
    bool placed;
    bool stepped;
    bool rendered;
} Dk1FirstLevelCheckpointResult;

static uint32_t checkpoint_target_x(const Dk1FirstLevelRoute *route,
                                    uint8_t percent) {
    uint32_t distance;
    uint32_t target;
    if (route == NULL)
        return 0u;
    distance = route->exit_x - route->start_x;
    target = route->start_x + distance * percent / 100u;
    if (target >= route->exit_x)
        target = route->exit_x - 1u;
    return target;
}

static bool try_place_x(Dk1SoftwareFrontend *frontend,
                        uint16_t height,
                        uint32_t candidate_x) {
    Dk1PlayerPreviewRuntime *player;
    if (frontend == NULL || candidate_x > UINT16_MAX)
        return false;
    player = &frontend->player_preview;
    player->motion.world_x = (uint16_t)candidate_x;
    player->motion.world_y = 0;
    player->motion.subpixel_x = 0u;
    player->motion.subpixel_y = 0u;
    player->motion.velocity_x = 0;
    player->motion.velocity_y = 0;
    player->motion.base_velocity_x = 0;
    player->motion.target_velocity_x = 0;
    player->airborne = true;
    return dk1_player_terrain_place(&frontend->player_terrain, player, height,
                                    &frontend->runtime.view.camera_y);
}

static bool place_near_target(Dk1SoftwareFrontend *frontend,
                              const Dk1FirstLevelRoute *route,
                              uint16_t height,
                              uint32_t target_x,
                              uint16_t *placed_x) {
    uint32_t radius;
    if (frontend == NULL || route == NULL || placed_x == NULL ||
        !frontend->player_terrain_ready)
        return false;

    if (try_place_x(frontend, height, target_x)) {
        *placed_x = (uint16_t)target_x;
        return true;
    }
    for (radius = DK1_FIRST_LEVEL_SEARCH_STEP;
         radius <= DK1_FIRST_LEVEL_SEARCH_RADIUS;
         radius += DK1_FIRST_LEVEL_SEARCH_STEP) {
        uint32_t candidate;
        if (target_x + radius < route->exit_x) {
            candidate = target_x + radius;
            if (try_place_x(frontend, height, candidate)) {
                *placed_x = (uint16_t)candidate;
                return true;
            }
        }
        if (target_x >= route->start_x + radius) {
            candidate = target_x - radius;
            if (try_place_x(frontend, height, candidate)) {
                *placed_x = (uint16_t)candidate;
                return true;
            }
        }
    }
    return false;
}

static size_t count_nonzero_pixels(const Dk1Rgba8 *pixels, size_t count) {
    size_t index;
    size_t nonzero = 0u;
    if (pixels == NULL)
        return 0u;
    for (index = 0u; index < count; ++index) {
        if (pixels[index].r != 0u || pixels[index].g != 0u ||
            pixels[index].b != 0u)
            ++nonzero;
    }
    return nonzero;
}

static bool validate_checkpoint(const Dk1RomImage *rom,
                                const Dk1SceneMemory *scene,
                                Dk1SoftwareFrontend *frontend,
                                Dk1FirstLevelRoute *route,
                                Dk1RgbaSurface surface,
                                uint8_t percent,
                                Dk1FirstLevelCheckpointResult *result) {
    uint32_t target_x;
    uint32_t camera_x;
    uint16_t placed_x;
    if (rom == NULL || scene == NULL || frontend == NULL || route == NULL ||
        result == NULL)
        return false;
    memset(result, 0, sizeof(*result));
    result->requested_percent = percent;
    target_x = checkpoint_target_x(route, percent);
    if (!place_near_target(frontend, route, DK1_FIRST_LEVEL_HEIGHT,
                           target_x, &placed_x))
        return false;
    result->placed = true;

    camera_x = placed_x > DK1_FIRST_LEVEL_WIDTH / 2u
        ? (uint32_t)placed_x - DK1_FIRST_LEVEL_WIDTH / 2u
        : 0u;
    if (camera_x > UINT16_MAX)
        return false;
    frontend->runtime.view.camera_x = (uint16_t)camera_x;
    if (!dk1_scene_view_clamp(scene, &frontend->runtime.view))
        return false;

    if (!dk1_software_frontend_step(scene, 0u, frontend))
        return false;
    result->stepped = true;
    if (!dk1_software_frontend_render(rom, scene, frontend, surface))
        return false;
    result->rendered = true;

    dk1_first_level_route_step(route,
                               frontend->player_preview.motion.world_x);
    result->world_x = frontend->player_preview.motion.world_x;
    result->world_y = frontend->player_preview.motion.world_y;
    result->camera_x = frontend->runtime.view.camera_x;
    result->camera_y = frontend->runtime.view.camera_y;
    result->active_objects = frontend->level_objects.active_count;
    result->active_gnawties = frontend->gnawty_active_count;
    result->active_barrels = frontend->streamed_barrels.active_count +
        ((frontend->barrel_ready && frontend->barrel.live.active) ? 1u : 0u);
    result->nonzero_pixels = count_nonzero_pixels(
        surface.pixels, surface.width * surface.height);
    if (frontend->dynamic_bg1 != NULL) {
        result->bg1_generation = frontend->dynamic_bg1->generation;
        result->bg1_columns =
            frontend->dynamic_bg1->map_columns_uploaded;
    }
    result->frame_signature = dk1_fnv1a64(
        surface.pixels,
        surface.width * surface.height * sizeof(*surface.pixels), 0u);

    return result->nonzero_pixels != 0u &&
           result->frame_signature != 0u &&
           frontend->level_objects_ready &&
           frontend->level_objects.catalog_count != 0u &&
           frontend->dynamic_bg1 != NULL &&
           frontend->dynamic_bg1->ready &&
           result->bg1_columns != 0u;
}

static void write_report(
    const char *path,
    const Dk1SceneMemory *scene,
    const Dk1FirstLevelRoute *route,
    const Dk1FirstLevelPreflight *preflight,
    const Dk1FirstLevelCheckpointResult *checkpoints,
    size_t checkpoint_count,
    uint64_t route_signature,
    size_t failures) {
    FILE *file;
    size_t index;
    if (path == NULL || scene == NULL || route == NULL || preflight == NULL ||
        checkpoints == NULL)
        return;
    file = fopen(path, "w");
    if (file == NULL)
        return;
    fprintf(file, "DK1 Jungle Hijinxs headless route validation\n");
    fprintf(file, "ready=%u\n", failures == 0u ? 1u : 0u);
    fprintf(file, "failures=%llu\n", (unsigned long long)failures);
    fprintf(file, "preflight_ready=%u\n", preflight->ready ? 1u : 0u);
    fprintf(file, "preflight_failure_mask=0x%08lX\n",
            (unsigned long)preflight->failure_mask);
    fprintf(file, "scene_cache_hit=%u\n", scene->cache.hit ? 1u : 0u);
    fprintf(file, "scene_packages=%llu\n",
            (unsigned long long)scene->assets.package_count);
    fprintf(file, "scene_dma_bytes=%llu\n",
            (unsigned long long)scene->assets.dma_bytes);
    fprintf(file, "route_start_x=%lu\n", (unsigned long)route->start_x);
    fprintf(file, "route_exit_x=%lu\n", (unsigned long)route->exit_x);
    fprintf(file, "route_furthest_x=%lu\n",
            (unsigned long)route->furthest_x);
    fprintf(file, "route_progress=%u\n",
            (unsigned)route->progress_percent);
    fprintf(file, "route_completed=%u\n", route->completed ? 1u : 0u);
    fprintf(file, "route_signature=%016llX\n",
            (unsigned long long)route_signature);
    fprintf(file, "checkpoint_count=%llu\n",
            (unsigned long long)checkpoint_count);
    for (index = 0u; index < checkpoint_count; ++index) {
        const Dk1FirstLevelCheckpointResult *item = &checkpoints[index];
        fprintf(file,
                "checkpoint_%llu=percent:%u placed:%u stepped:%u rendered:%u "
                "x:%u y:%d camera:%u,%u objects:%llu gnawties:%llu "
                "barrels:%llu pixels:%llu bg1_generation:%lu "
                "bg1_columns:%llu frame:%016llX\n",
                (unsigned long long)index,
                (unsigned)item->requested_percent,
                item->placed ? 1u : 0u,
                item->stepped ? 1u : 0u,
                item->rendered ? 1u : 0u,
                (unsigned)item->world_x,
                (int)item->world_y,
                (unsigned)item->camera_x,
                (unsigned)item->camera_y,
                (unsigned long long)item->active_objects,
                (unsigned long long)item->active_gnawties,
                (unsigned long long)item->active_barrels,
                (unsigned long long)item->nonzero_pixels,
                (unsigned long)item->bg1_generation,
                (unsigned long long)item->bg1_columns,
                (unsigned long long)item->frame_signature);
    }
    fclose(file);
}

int main(int argc, char **argv) {
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom = {0};
    Dk1RomIdentity identity;
    Dk1SceneMemory *scene = NULL;
    Dk1SoftwareFrontend frontend;
    Dk1LevelSoftwareFrontendStats source_stats;
    Dk1PreviewAssetWarmupStats warmup;
    Dk1FirstLevelRoute route;
    Dk1FirstLevelPreflight preflight;
    Dk1FirstLevelCheckpointResult
        checkpoints[DK1_FIRST_LEVEL_CHECKPOINT_COUNT];
    Dk1Rgba8 *pixels = NULL;
    Dk1RgbaSurface surface;
    char cache_path[DK1_SCENE_ASSET_CACHE_PATH_CAPACITY];
    const char *report_path;
    uint64_t route_signature = 0u;
    size_t failures = 0u;
    size_t index;
    int result = 1;

    memset(&frontend, 0, sizeof(frontend));
    memset(&source_stats, 0, sizeof(source_stats));
    memset(&warmup, 0, sizeof(warmup));
    memset(&route, 0, sizeof(route));
    memset(&preflight, 0, sizeof(preflight));
    memset(checkpoints, 0, sizeof(checkpoints));
    cache_path[0] = '\0';

    if (argc < 2 || argc > 3) {
        fprintf(stderr, "usage: %s ROM [REPORT]\n", argv[0]);
        return 2;
    }
    report_path = argc == 3
        ? argv[2]
        : "DK1-Jungle-Hijinxs-Route-Validation.txt";

    scene = (Dk1SceneMemory *)calloc(1u, sizeof(*scene));
    pixels = (Dk1Rgba8 *)calloc(
        DK1_FIRST_LEVEL_WIDTH * DK1_FIRST_LEVEL_HEIGHT, sizeof(*pixels));
    if (scene == NULL || pixels == NULL) {
        fputs("allocation failure\n", stderr);
        goto cleanup;
    }
    surface = (Dk1RgbaSurface){
        pixels, DK1_FIRST_LEVEL_WIDTH, DK1_FIRST_LEVEL_HEIGHT,
        DK1_FIRST_LEVEL_WIDTH
    };

    if (!dk1_rom_load_file(argv[1], &owned, &rom) ||
        !dk1_rom_identity(&rom, &identity) ||
        !dk1_rom_identity_is_supported_rev2(&identity)) {
        fputs("unsupported ROM\n", stderr);
        goto cleanup;
    }
    if (!dk1_scene_asset_cache_default_path(
            argv[1], DK1_FIRST_LEVEL_ID, false, false,
            cache_path, sizeof(cache_path)))
        cache_path[0] = '\0';
    if (!dk1_scene_memory_load_cached(
            &rom, DK1_FIRST_LEVEL_ID, false, false,
            cache_path[0] != '\0' ? cache_path : NULL, scene) ||
        !dk1_level_software_frontend_init(
            &rom, scene, DK1_FIRST_LEVEL_ID,
            DK1_FIRST_LEVEL_WIDTH, DK1_FIRST_LEVEL_HEIGHT,
            &frontend, &source_stats) ||
        !dk1_preview_asset_warmup(
            &rom, scene, &frontend, surface, &warmup) ||
        !dk1_first_level_route_init(
            scene, DK1_FIRST_LEVEL_WIDTH,
            frontend.player_preview.motion.world_x, &route)) {
        fputs("first-level initialization failed\n", stderr);
        goto cleanup;
    }
    if (!dk1_first_level_preflight(
            scene, &frontend, &warmup, &route, &preflight)) {
        ++failures;
    }

    for (index = 0u; index < DK1_FIRST_LEVEL_CHECKPOINT_COUNT; ++index) {
        memset(pixels, 0, DK1_FIRST_LEVEL_WIDTH * DK1_FIRST_LEVEL_HEIGHT *
                              sizeof(*pixels));
        if (!validate_checkpoint(
                &rom, scene, &frontend, &route, surface,
                checkpoint_percent[index], &checkpoints[index])) {
            ++failures;
            fprintf(stderr, "checkpoint %u failed\n",
                    (unsigned)checkpoint_percent[index]);
        }
        route_signature = dk1_fnv1a64(
            &checkpoints[index], sizeof(checkpoints[index]), route_signature);
    }

    dk1_first_level_route_step(&route, route.exit_x);
    if (!route.completed || route.progress_percent != 100u ||
        route.checkpoints_reached != 4u)
        ++failures;

    write_report(report_path, scene, &route, &preflight,
                 checkpoints, DK1_FIRST_LEVEL_CHECKPOINT_COUNT,
                 route_signature, failures);
    printf("first_level_ready=%u failures=%llu checkpoints=%u "
           "route=%u%% completed=%u signature=%016llX report=%s\n",
           failures == 0u ? 1u : 0u,
           (unsigned long long)failures,
           (unsigned)DK1_FIRST_LEVEL_CHECKPOINT_COUNT,
           (unsigned)route.progress_percent,
           route.completed ? 1u : 0u,
           (unsigned long long)route_signature,
           report_path);
    result = failures == 0u ? 0 : 1;

cleanup:
    dk1_software_frontend_dispose(&frontend);
    free(pixels);
    free(scene);
    dk1_rom_free(&owned);
    return result;
}
