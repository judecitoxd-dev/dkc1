#include "dk1/first_level_route.h"
#include <string.h>

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
