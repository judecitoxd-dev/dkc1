#ifndef DK1_INPUT_H
#define DK1_INPUT_H

#include <stdbool.h>
#include <stdint.h>

#define DK1_INPUT_PLAYER_COUNT 2u

enum Dk1Button {
    DK1_BUTTON_B      = 0x8000u,
    DK1_BUTTON_Y      = 0x4000u,
    DK1_BUTTON_SELECT = 0x2000u,
    DK1_BUTTON_START  = 0x1000u,
    DK1_BUTTON_UP     = 0x0800u,
    DK1_BUTTON_DOWN   = 0x0400u,
    DK1_BUTTON_LEFT   = 0x0200u,
    DK1_BUTTON_RIGHT  = 0x0100u,
    DK1_BUTTON_A      = 0x0080u,
    DK1_BUTTON_X      = 0x0040u,
    DK1_BUTTON_L      = 0x0020u,
    DK1_BUTTON_R      = 0x0010u,
};

typedef struct Dk1InputState {
    uint16_t held[DK1_INPUT_PLAYER_COUNT];
    uint16_t pressed[DK1_INPUT_PLAYER_COUNT];
    uint16_t active_held;
    uint16_t active_pressed;
} Dk1InputState;

/* Semantic model of $00:C180-$00:C1AF. */
void dk1_input_update_auto(Dk1InputState *state, uint16_t joy1, uint16_t joy2);

/* Semantic model of $00:C1B2-$00:C20A. */
void dk1_input_select_active(
    Dk1InputState *state,
    uint16_t player_mode,
    uint16_t selection_index,
    uint16_t active_player_or_mode
);

bool dk1_input_held(const Dk1InputState *state, uint16_t buttons);
bool dk1_input_pressed(const Dk1InputState *state, uint16_t buttons);

#endif
