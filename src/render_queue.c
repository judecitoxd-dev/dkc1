#include "dk1/render_queue.h"
#include "dk1/object_runtime.h"

static bool visible(int32_t x, int32_t y, Dk1Viewport view) {
    const int64_t left = -(int64_t)view.margin;
    const int64_t top = -(int64_t)view.margin;
    const int64_t right = (int64_t)view.width + view.margin;
    const int64_t bottom = (int64_t)view.height + view.margin;
    return x >= left && y >= top && x < right && y < bottom;
}

static void stable_sort(Dk1RenderQueue *queue) {
    size_t i = 0u;
    for (i = 1u; i < queue->count; ++i) {
        Dk1RenderObject value = queue->objects[i];
        size_t j = i;
        while (j > 0u && queue->objects[j - 1u].priority > value.priority) {
            queue->objects[j] = queue->objects[j - 1u];
            --j;
        }
        queue->objects[j] = value;
    }
}

void dk1_render_queue_build(
    Dk1RenderQueue *queue,
    const Dk1ObjectWorld *world,
    Dk1Viewport viewport
) {
    size_t slot = 0u;
    if (queue == 0) return;
    queue->count = 0u;
    if (world == 0 || viewport.width == 0u || viewport.height == 0u) return;

    for (slot = 0u; slot < DK1_OBJECT_SLOT_COUNT; ++slot) {
        const uint16_t type_id = world->scheduler.type_id[slot];
        Dk1ObjectTypeDispatch dispatch = {0};
        Dk1RenderObject object;
        if (type_id == 0u ||
            !dk1_object_type_dispatch_get(type_id, &dispatch)) continue;
        object.screen_x = world->world_x[slot] - viewport.x;
        object.screen_y = world->world_y[slot] - viewport.y;
        if (!visible(object.screen_x, object.screen_y, viewport)) continue;
        object.slot = slot;
        object.type_id = type_id;
        object.priority = dk1_object_type_priority(dispatch);
        object.callback_pc = ((uint32_t)DK1_OBJECT_CALLBACK_BANK << 16u) |
                             dispatch.callback_address_bf;
        queue->objects[queue->count++] = object;
    }
    stable_sort(queue);
}
