#ifndef DK1_PLAYER_EVENTS_H
#define DK1_PLAYER_EVENTS_H

#include <stdbool.h>
#include <stdint.h>

typedef enum Dk1PlayerEventAction {
    DK1_PLAYER_EVENT_NONE = 0,
    DK1_PLAYER_EVENT_STATE_11,
    DK1_PLAYER_EVENT_STATE_4E,
    DK1_PLAYER_EVENT_STATE_55
} Dk1PlayerEventAction;

typedef struct Dk1PlayerEventResult {
    bool consumed;
    uint16_t new_state;
    uint16_t global_051a;
    bool clear_aux_11a1;
    bool call_fd8c;
    bool call_a29b;
    bool call_fda6;
    Dk1PlayerEventAction action;
} Dk1PlayerEventResult;

Dk1PlayerEventResult dk1_player_event_transition(uint16_t event_code,
                                                 uint16_t flags_12a5,
                                                 uint16_t global_1e15);

#endif
