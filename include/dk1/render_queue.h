#ifndef DK1_RENDER_QUEUE_H
#define DK1_RENDER_QUEUE_H

#include <stddef.h>
#include <stdint.h>
#include "dk1/object_executor.h"

#define DK1_RENDER_QUEUE_CAPACITY DK1_OBJECT_SLOT_COUNT

typedef struct Dk1Viewport {
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;
    int32_t margin;
} Dk1Viewport;

typedef struct Dk1RenderObject {
    size_t slot;
    uint16_t type_id;
    int32_t screen_x;
    int32_t screen_y;
    uint8_t priority;
    uint32_t callback_pc;
} Dk1RenderObject;

typedef struct Dk1RenderQueue {
    Dk1RenderObject objects[DK1_RENDER_QUEUE_CAPACITY];
    size_t count;
} Dk1RenderQueue;

void dk1_render_queue_build(
    Dk1RenderQueue *queue,
    const Dk1ObjectWorld *world,
    Dk1Viewport viewport
);

#endif
