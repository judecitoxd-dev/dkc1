#ifndef DK1_PLAYER_INPUT_ROUTER_H
#define DK1_PLAYER_INPUT_ROUTER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DK1_PLAYER_INPUT_ACTION_CAPACITY 11u

typedef enum Dk1PlayerInputAction {
    DK1_PLAYER_INPUT_VERTICAL_NEUTRAL = 0,
    DK1_PLAYER_INPUT_UP,
    DK1_PLAYER_INPUT_DOWN,
    DK1_PLAYER_INPUT_HORIZONTAL_NEUTRAL,
    DK1_PLAYER_INPUT_LEFT,
    DK1_PLAYER_INPUT_RIGHT,
    DK1_PLAYER_INPUT_SELECT,
    DK1_PLAYER_INPUT_A_RELEASED,
    DK1_PLAYER_INPUT_A_HELD,
    DK1_PLAYER_INPUT_Y,
    DK1_PLAYER_INPUT_X_RELEASED,
    DK1_PLAYER_INPUT_X_HELD,
    DK1_PLAYER_INPUT_L,
    DK1_PLAYER_INPUT_R,
    DK1_PLAYER_INPUT_B_RELEASED,
    DK1_PLAYER_INPUT_B_HELD,
    DK1_PLAYER_INPUT_START
} Dk1PlayerInputAction;

typedef struct Dk1PlayerInputRoute {
    uint16_t mode_argument_180f;
    uint16_t held;
    uint16_t pressed;
    uint16_t flags_1917;
    Dk1PlayerInputAction actions[DK1_PLAYER_INPUT_ACTION_CAPACITY];
    size_t action_count;
} Dk1PlayerInputRoute;

bool dk1_player_input_route(uint16_t mode_argument,
                            uint16_t held,
                            uint16_t pressed,
                            uint16_t flags_1917,
                            Dk1PlayerInputRoute *route);

#endif
