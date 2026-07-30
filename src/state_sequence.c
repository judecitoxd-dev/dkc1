#include "dk1/state_sequence.h"

const uint16_t *dk1_initial_boot_sequence(size_t *count) {
    static const uint16_t callbacks[] = {
        0x822C, 0x8279, 0x9723, 0x8279, 0x9ADA, 0x8279, 0x9ADA,
    };
    if (count != NULL) {
        *count = sizeof(callbacks) / sizeof(callbacks[0]);
    }
    return callbacks;
}

const uint16_t *dk1_short_boot_sequence(size_t *count) {
    static const uint16_t callbacks[] = {0x82D2, 0x9723};
    if (count != NULL) {
        *count = sizeof(callbacks) / sizeof(callbacks[0]);
    }
    return callbacks;
}

bool dk1_state_sequence_begin(Dk1StateSequence *sequence,
                              uint16_t table_base_address,
                              const uint16_t *callbacks,
                              size_t callback_count) {
    if (sequence == NULL || callbacks == NULL || callback_count == 0) {
        return false;
    }
    sequence->table_base_address = table_base_address;
    sequence->element_index = 0;
    sequence->current_callback = callbacks[0];
    return true;
}

bool dk1_state_sequence_advance(Dk1StateSequence *sequence,
                                const uint16_t *callbacks,
                                size_t callback_count) {
    size_t next_index;
    if (sequence == NULL || callbacks == NULL) {
        return false;
    }
    next_index = (size_t)sequence->element_index + 1u;
    if (next_index >= callback_count || next_index > UINT16_MAX) {
        return false;
    }
    sequence->element_index = (uint16_t)next_index;
    sequence->current_callback = callbacks[next_index];
    return true;
}
