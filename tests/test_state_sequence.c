#include <assert.h>
#include <stddef.h>
#include "dk1/state_sequence.h"

int main(void) {
    Dk1StateSequence sequence = {0};
    size_t count = 0;
    const uint16_t *callbacks = dk1_initial_boot_sequence(&count);
    assert(count == 7u);
    assert(callbacks[0] == 0x822Cu);
    assert(callbacks[6] == 0x9ADAu);
    assert(dk1_state_sequence_begin(&sequence, 0x81E1u, callbacks, count));
    assert(sequence.table_base_address == 0x81E1u);
    assert(sequence.element_index == 0u);
    assert(sequence.current_callback == 0x822Cu);
    assert(dk1_state_sequence_advance(&sequence, callbacks, count));
    assert(sequence.element_index == 1u);
    assert(sequence.current_callback == 0x8279u);

    callbacks = dk1_short_boot_sequence(&count);
    assert(count == 2u);
    assert(dk1_state_sequence_begin(&sequence, 0x81EFu, callbacks, count));
    assert(dk1_state_sequence_advance(&sequence, callbacks, count));
    assert(sequence.current_callback == 0x9723u);
    assert(!dk1_state_sequence_advance(&sequence, callbacks, count));
    return 0;
}
