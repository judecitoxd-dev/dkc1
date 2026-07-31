#include <assert.h>
#include "dk1/render_queue.h"

int main(void) {
    Dk1ObjectWorld world = {0};
    Dk1RenderQueue queue;
    Dk1Viewport view = {100, 50, 320u, 224u, 16};
    world.scheduler.type_id[1] = 2u;
    world.world_x[1] = 120;
    world.world_y[1] = 70;
    world.scheduler.type_id[2] = 16u;
    world.world_x[2] = 130;
    world.world_y[2] = 80;
    world.scheduler.type_id[3] = 3u;
    world.world_x[3] = 1000;
    world.world_y[3] = 1000;
    dk1_render_queue_build(&queue, &world, view);
    assert(queue.count == 2u);
    assert(queue.objects[0].priority <= queue.objects[1].priority);
    assert(queue.objects[0].screen_x == 20 || queue.objects[0].screen_x == 30);
    return 0;
}
