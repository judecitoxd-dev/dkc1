#include "dk1/input.h"

void dk1_input_update_auto(Dk1InputState *state, uint16_t joy1, uint16_t joy2) {
    const uint16_t next[DK1_INPUT_PLAYER_COUNT] = {joy1, joy2};
    unsigned i = 0u;
    if (state == 0) return;
    for (i = 0u; i < DK1_INPUT_PLAYER_COUNT; ++i) {
        state->pressed[i] = (uint16_t)((next[i] ^ state->held[i]) & next[i]);
        state->held[i] = next[i];
    }
}

void dk1_input_select_active(
    Dk1InputState *state,
    uint16_t player_mode,
    uint16_t selection_index,
    uint16_t active_player_or_mode
) {
    unsigned index = 0u;
    bool merge_players = false;
    if (state == 0) return;

    if (player_mode == 2u) {
        index = (unsigned)(selection_index & 1u);
        merge_players = true;
    } else if (player_mode == 1u) {
        index = (unsigned)((active_player_or_mode & 2u) >> 1u);
        merge_players = true;
    }

    state->active_held = state->held[index];
    state->active_pressed = state->pressed[index];

    if (merge_players) {
        state->held[0] = (uint16_t)(state->held[0] | state->held[1]);
        state->pressed[0] = (uint16_t)(state->pressed[0] | state->pressed[1]);
    }
}

bool dk1_input_held(const Dk1InputState *state, uint16_t buttons) {
    return state != 0 && (state->active_held & buttons) == buttons;
}

bool dk1_input_pressed(const Dk1InputState *state, uint16_t buttons) {
    return state != 0 && (state->active_pressed & buttons) == buttons;
}
