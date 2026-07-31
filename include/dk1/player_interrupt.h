#ifndef DK1_PLAYER_INTERRUPT_H
#define DK1_PLAYER_INTERRUPT_H
#include <stdbool.h>
#include <stdint.h>

typedef struct Dk1PlayerInterruptState {
    uint16_t request_1595;
    uint16_t state_1029;
    uint16_t field_11a1;
    uint16_t aux_16e5;
    uint16_t attached_object_0512;
    uint16_t aux_16f5;
} Dk1PlayerInterruptState;

typedef struct Dk1PlayerInterruptResult {
    bool abort_state;
    uint16_t effect_id;
    uint16_t spawn_type;
    bool call_a1b9;
    bool call_a29b;
} Dk1PlayerInterruptResult;

bool dk1_player_interrupt_apply(Dk1PlayerInterruptState *state,
                                Dk1PlayerInterruptResult *result);
#endif
