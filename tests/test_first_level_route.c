#include <assert.h>
#include <string.h>
#include "dk1/first_level_route.h"

int main(void) {
    Dk1SceneMemory scene;
    Dk1FirstLevelRoute route;
    uint32_t midpoint;

    memset(&scene, 0, sizeof(scene));
    scene.camera.maximum_x = 1000u;
    assert(dk1_first_level_route_init(&scene, 384u, 128u, &route));
    assert(route.start_x == 128u);
    assert(route.furthest_x == 128u);
    assert(route.exit_x == 1320u);
    assert(route.progress_percent == 0u);
    assert(route.checkpoints_reached == 0u);
    assert(!route.completed);

    dk1_first_level_route_step(&route, 64u);
    assert(route.progress_percent == 0u);
    assert(route.furthest_x == 128u);
    assert(!route.completed);

    midpoint = route.start_x + (route.exit_x - route.start_x) / 2u;
    dk1_first_level_route_step(&route, midpoint);
    assert(route.progress_percent >= 49u);
    assert(route.progress_percent <= 50u);
    assert(route.checkpoints_reached == 2u);
    assert(!route.completed);

    /* Backtracking must not lower the tester's route progress. */
    dk1_first_level_route_step(&route, route.start_x + 8u);
    assert(route.progress_percent >= 49u);
    assert(route.progress_percent <= 50u);
    assert(route.furthest_x == midpoint);
    assert(route.checkpoints_reached == 2u);

    dk1_first_level_route_step(&route, route.exit_x - 1u);
    assert(route.progress_percent == 99u);
    assert(route.checkpoints_reached == 3u);
    assert(!route.completed);

    dk1_first_level_route_step(&route, route.exit_x);
    assert(route.progress_percent == 100u);
    assert(route.checkpoints_reached == 4u);
    assert(route.completed);
    assert(route.frames == 5u);
    return 0;
}
