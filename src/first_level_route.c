#include "dk1/first_level_route.h"
#include <string.h>
#include "dk1/preview_asset_warmup.h"
#include "dk1/software_frontend.h"

bool dk1_first_level_route_init(const Dk1SceneMemory *scene,
                                uint16_t viewport_width,
                                uint32_t player_start_x,
                                Dk1FirstLevelRoute *route) {
    uint32_t exit_x;
    if (scene == NULL || route == NULL || viewport_width < 64u)
        return false;
    memset(route, 0, sizeof(*route));
    exit_x = (uint32_t)scene->camera.maximum_x +
             (uint32_t)viewport_width - 64u;
    if (exit_x <= player_start_x)
        exit_x = player_start_x + 1u;
    route->start_x = player_start_x;
    route->current_x = player_start_x;
    route->furthest_x = player_start_x;
    route->exit_x = exit_x;
    return true;
}

void dk1_first_level_route_step(Dk1FirstLevelRoute *route,
                                uint32_t player_x) {
    uint32_t travelled;
    uint32_t distance;
    uint32_t percent;
    uint8_t checkpoints;
    if (route == NULL)
        return;
    ++route->frames;
    route->current_x = player_x;
    if (player_x > route->furthest_x)
        route->furthest_x = player_x;
    if (route->furthest_x >= route->exit_x) {
        route->furthest_x = route->exit_x;
        route->progress_percent = 100u;
        route->checkpoints_reached = 4u;
        route->completed = true;
        return;
    }
    if (route->furthest_x <= route->start_x) {
        route->progress_percent = 0u;
        return;
    }
    travelled = route->furthest_x - route->start_x;
    distance = route->exit_x - route->start_x;
    percent = distance != 0u ? travelled * 100u / distance : 100u;
    if (percent > 99u)
        percent = 99u;
    route->progress_percent = (uint8_t)percent;
    checkpoints = (uint8_t)(percent / 25u);
    if (checkpoints > 3u)
        checkpoints = 3u;
    if (checkpoints > route->checkpoints_reached)
        route->checkpoints_reached = checkpoints;
}

bool dk1_first_level_preflight(
    const Dk1SceneMemory *scene,
    const struct Dk1SoftwareFrontend *frontend,
    const struct Dk1PreviewAssetWarmupStats *warmup,
    const Dk1FirstLevelRoute *route,
    Dk1FirstLevelPreflight *result) {
    const Dk1SoftwareFrontend *typed_frontend = frontend;
    const Dk1PreviewAssetWarmupStats *typed_warmup = warmup;

    if (scene == NULL || typed_frontend == NULL || typed_warmup == NULL ||
        route == NULL || result == NULL)
        return false;

    memset(result, 0, sizeof(*result));
    result->scene_packages = scene->assets.package_count;
    result->scene_dma_bytes = scene->assets.dma_bytes;
    result->player_dma_records = typed_warmup->player_dma_records;
    result->player_dma_bytes = typed_warmup->player_dma_bytes;
    if (route->exit_x > route->start_x)
        result->route_distance = route->exit_x - route->start_x;

    if (typed_frontend->dynamic_bg1 != NULL) {
        result->initial_map_columns =
            typed_frontend->dynamic_bg1->map_columns_uploaded;
        result->initial_tile_words =
            typed_frontend->dynamic_bg1->tile_words_uploaded;
    }

    if (!scene->assets.textures_loaded ||
        !typed_warmup->scene_textures_ready)
        result->failure_mask |= DK1_FIRST_LEVEL_PREFLIGHT_SCENE_TEXTURES;
    if (!scene->assets.palettes_loaded ||
        !typed_warmup->scene_palettes_ready)
        result->failure_mask |= DK1_FIRST_LEVEL_PREFLIGHT_SCENE_PALETTES;
    if (scene->assets.package_count == 0u || scene->assets.dma_bytes == 0u ||
        typed_warmup->scene_packages == 0u ||
        typed_warmup->scene_dma_bytes == 0u)
        result->failure_mask |= DK1_FIRST_LEVEL_PREFLIGHT_SCENE_PACKAGES;
    if (typed_frontend->source_rom == NULL)
        result->failure_mask |= DK1_FIRST_LEVEL_PREFLIGHT_SOURCE_ROM;
    if (!typed_frontend->player_terrain_ready)
        result->failure_mask |= DK1_FIRST_LEVEL_PREFLIGHT_PLAYER_TERRAIN;
    if (!typed_frontend->level_objects_ready)
        result->failure_mask |= DK1_FIRST_LEVEL_PREFLIGHT_OBJECT_STREAM;
    if (typed_frontend->dynamic_bg1 == NULL ||
        !typed_frontend->dynamic_bg1->ready ||
        !typed_warmup->dynamic_bg1_ready ||
        result->initial_map_columns == 0u ||
        result->initial_tile_words == 0u)
        result->failure_mask |= DK1_FIRST_LEVEL_PREFLIGHT_DYNAMIC_BG1;
    if (!typed_warmup->player_visual_ready ||
        typed_warmup->player_dma_records == 0u ||
        typed_warmup->player_dma_bytes == 0u)
        result->failure_mask |= DK1_FIRST_LEVEL_PREFLIGHT_PLAYER_VISUAL;
    if (typed_frontend->runtime.view.width == 0u ||
        typed_frontend->runtime.view.height == 0u)
        result->failure_mask |= DK1_FIRST_LEVEL_PREFLIGHT_VIEW;
    if (route->exit_x <= route->start_x ||
        route->furthest_x < route->start_x ||
        route->progress_percent != 0u || route->completed)
        result->failure_mask |= DK1_FIRST_LEVEL_PREFLIGHT_ROUTE;

    result->ready = result->failure_mask == 0u;
    return result->ready;
}
