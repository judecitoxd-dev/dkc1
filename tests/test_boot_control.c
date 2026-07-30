#include <assert.h>
#include "dk1/boot_control.h"

int main(void) {
    Dk1BootControlState state = dk1_boot_control_state(0);
    assert(state.player_or_mode_index_0042 == 0);
    assert(state.selection_index_0044 == 0);
    assert(state.active_player_056f == 1);
    assert(state.boot_counter_003c == 0x002C);
    assert(state.raw_0016 == 0x3200);
    assert(state.raw_0018 == 0x3300);
    assert(state.init_request_id == 0x001B);
    assert(state.followup_request_id == 0x000A);

    state = dk1_boot_control_state(1);
    assert(state.raw_0016 == 0);
    assert(state.raw_0018 == 0);
    assert(state.init_request_id == 0x000A);
    return 0;
}
