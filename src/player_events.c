#include "dk1/player_events.h"

Dk1PlayerEventResult dk1_player_event_transition(uint16_t event_code,
                                                 uint16_t flags_12a5,
                                                 uint16_t global_1e15) {
    Dk1PlayerEventResult result = {0};
    if (event_code == 4u) {
        result.consumed = true;
        result.new_state = 0x004Eu;
        result.global_051a = 0x820Fu;
        result.action = DK1_PLAYER_EVENT_STATE_4E;
        return result;
    }
    if (event_code == 3u) {
        if ((flags_12a5 & 1u) == 0u) return result;
        result.consumed = true;
        result.new_state = 0x0055u;
        result.clear_aux_11a1 = true;
        result.call_fd8c = true;
        result.call_a29b = (global_1e15 & 0x0020u) == 0u;
        result.action = DK1_PLAYER_EVENT_STATE_55;
        return result;
    }
    if (event_code == 1u) {
        if ((flags_12a5 & 1u) == 0u) return result;
        result.consumed = true;
        result.new_state = 0x0011u;
        result.clear_aux_11a1 = true;
        result.call_fd8c = true;
        result.call_a29b = (global_1e15 & 0x0020u) == 0u;
        result.action = DK1_PLAYER_EVENT_STATE_11;
        return result;
    }
    if (event_code == 2u) {
        if ((flags_12a5 & 0x1001u) != 0x0001u) return result;
        result.consumed = true;
        result.new_state = 0x0011u;
        result.clear_aux_11a1 = true;
        result.call_fda6 = true;
        result.action = DK1_PLAYER_EVENT_STATE_11;
    }
    return result;
}
