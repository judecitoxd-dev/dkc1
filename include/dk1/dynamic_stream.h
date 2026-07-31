#ifndef DK1_DYNAMIC_STREAM_H
#define DK1_DYNAMIC_STREAM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/scene_view.h"

typedef struct Dk1DynamicStreamState {
    bool initialized;
    uint16_t first_column;
    uint16_t last_column;
    uint32_t generation;
    uint16_t entered_left;
    uint16_t entered_right;
} Dk1DynamicStreamState;

typedef struct Dk1DynamicStreamUpdate {
    bool changed;
    uint16_t old_first;
    uint16_t old_last;
    uint16_t new_first;
    uint16_t new_last;
    uint16_t columns_entered;
    uint16_t columns_left;
} Dk1DynamicStreamUpdate;

bool dk1_dynamic_stream_update(Dk1SceneView view, size_t map_columns,
                               Dk1DynamicStreamState *state,
                               Dk1DynamicStreamUpdate *update);

#endif
