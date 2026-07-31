#ifndef DK1_OBJECT_EXECUTOR_H
#define DK1_OBJECT_EXECUTOR_H

#include <stddef.h>
#include <stdint.h>
#include "dk1/host_callback.h"
#include "dk1/object_scheduler.h"

typedef struct Dk1ObjectWorld {
    Dk1ObjectFrameState scheduler;
    int32_t world_x[DK1_OBJECT_SLOT_COUNT];
    int32_t world_y[DK1_OBJECT_SLOT_COUNT];
    int32_t velocity_x[DK1_OBJECT_SLOT_COUNT];
    int32_t velocity_y[DK1_OBJECT_SLOT_COUNT];
    uint16_t flags[DK1_OBJECT_SLOT_COUNT];
} Dk1ObjectWorld;

typedef struct Dk1ObjectExecutionResult {
    Dk1ObjectFrameResult schedule;
    size_t executed_callbacks;
    size_t missing_callbacks;
    size_t failed_callbacks;
} Dk1ObjectExecutionResult;

Dk1ObjectExecutionResult dk1_object_execute_frame(
    Dk1ObjectWorld *world,
    const Dk1HostCallbackRegistry *registry,
    uint64_t frame_number
);

#endif
