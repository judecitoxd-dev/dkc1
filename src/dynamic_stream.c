#include "dk1/dynamic_stream.h"

bool dk1_dynamic_stream_update(Dk1SceneView view, size_t map_columns,
                               Dk1DynamicStreamState *state,
                               Dk1DynamicStreamUpdate *update) {
    uint32_t first, last;
    uint16_t entered = 0u, left = 0u;
    if (state == NULL || update == NULL || map_columns == 0u || view.width == 0u) return false;
    first = (uint32_t)view.camera_x / 32u;
    last = ((uint32_t)view.camera_x + view.width - 1u) / 32u;
    if (first >= map_columns) first = map_columns - 1u;
    if (last >= map_columns) last = map_columns - 1u;
    *update = (Dk1DynamicStreamUpdate){0};
    update->new_first = (uint16_t)first;
    update->new_last = (uint16_t)last;
    if (!state->initialized) {
        update->changed = true;
        update->old_first = (uint16_t)first;
        update->old_last = (uint16_t)first;
        update->columns_entered = (uint16_t)(last - first + 1u);
        state->initialized = true;
    } else {
        update->old_first = state->first_column;
        update->old_last = state->last_column;
        if (first > state->first_column) left += (uint16_t)(first - state->first_column);
        if (last < state->last_column) left += (uint16_t)(state->last_column - last);
        if (first < state->first_column) entered += (uint16_t)(state->first_column - first);
        if (last > state->last_column) entered += (uint16_t)(last - state->last_column);
        update->columns_entered = entered;
        update->columns_left = left;
        update->changed = entered != 0u || left != 0u;
    }
    state->entered_left = first < state->first_column ? (uint16_t)(state->first_column - first) : 0u;
    state->entered_right = last > state->last_column ? (uint16_t)(last - state->last_column) : 0u;
    state->first_column = (uint16_t)first;
    state->last_column = (uint16_t)last;
    if (update->changed) ++state->generation;
    return true;
}
