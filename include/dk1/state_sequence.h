#ifndef DK1_STATE_SEQUENCE_H
#define DK1_STATE_SEQUENCE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct Dk1StateSequence {
    uint16_t table_base_address;
    uint16_t element_index;
    uint16_t current_callback;
} Dk1StateSequence;

const uint16_t *dk1_initial_boot_sequence(size_t *count);
const uint16_t *dk1_short_boot_sequence(size_t *count);

bool dk1_state_sequence_begin(Dk1StateSequence *sequence,
                              uint16_t table_base_address,
                              const uint16_t *callbacks,
                              size_t callback_count);

bool dk1_state_sequence_advance(Dk1StateSequence *sequence,
                                const uint16_t *callbacks,
                                size_t callback_count);

#endif
