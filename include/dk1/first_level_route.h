#ifndef DK1_FIRST_LEVEL_ROUTE_H
#define DK1_FIRST_LEVEL_ROUTE_H

#include <stdbool.h>
#include <stdint.h>
#include "dk1/scene_memory.h"

typedef struct Dk1FirstLevelRoute {
    uint32_t start_x;
    uint32_t exit_x;
    uint32_t current_x;
    uint32_t furthest_x;
    uint32_t frames;
    uint8_t progress_percent;
    uint8_t checkpoints_reached;
    bool completed;
} Dk1FirstLevelRoute;

/* Portable completion gate for the first public test. The original level-exit
 * callback is still pending; this tracker derives a stable end point from the
 * translated camera bounds so the whole current route can be tested now. */
bool dk1_first_level_route_init(const Dk1SceneMemory *scene,
                                uint16_t viewport_width,
                                uint32_t player_start_x,
                                Dk1FirstLevelRoute *route);

/* Progress uses a high-water mark, so walking backwards never lowers the
 * displayed route percentage. checkpoints_reached reports the 25/50/75/100
 * percent milestones crossed during the current run. */
void dk1_first_level_route_step(Dk1FirstLevelRoute *route,
                                uint32_t player_x);

#endif
