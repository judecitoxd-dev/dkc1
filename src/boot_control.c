#include "dk1/boot_control.h"

Dk1BootControlState dk1_boot_control_state(uint16_t raw_flag_0525) {
    Dk1BootControlState state = {
        .player_or_mode_index_0042 = 0,
        .selection_index_0044 = 0,
        .active_player_056f = 1,
        .boot_counter_003c = 0x002C,
        .frame_counter_0028 = 0,
        .raw_0016 = 0,
        .raw_0018 = 0,
        .raw_001a = 0,
        .init_request_id = 0x000A,
        .followup_request_id = 0x000A,
    };

    if (raw_flag_0525 == 0) {
        state.raw_0016 = 0x3200;
        state.raw_0018 = 0x3300;
        state.init_request_id = 0x001B;
    }
    return state;
}
