#include <assert.h>
#include "dk1/input.h"

int main(void) {
    Dk1InputState state = {0};
    dk1_input_update_auto(&state, DK1_BUTTON_RIGHT | DK1_BUTTON_B, DK1_BUTTON_START);
    dk1_input_select_active(&state, 0u, 0u, 0u);
    assert(dk1_input_held(&state, DK1_BUTTON_RIGHT));
    assert(dk1_input_pressed(&state, DK1_BUTTON_B));
    dk1_input_update_auto(&state, DK1_BUTTON_RIGHT, DK1_BUTTON_START | DK1_BUTTON_A);
    dk1_input_select_active(&state, 2u, 1u, 0u);
    assert(dk1_input_pressed(&state, DK1_BUTTON_A));
    assert((state.held[0] & DK1_BUTTON_START) != 0u);
    assert(!dk1_input_pressed(&state, DK1_BUTTON_START));
    return 0;
}
