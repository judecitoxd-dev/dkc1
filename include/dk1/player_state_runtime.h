#ifndef DK1_PLAYER_STATE_RUNTIME_H
#define DK1_PLAYER_STATE_RUNTIME_H

#include <stdbool.h>
#include <stdint.h>

#define DK1_PLAYER_CALL_AA08   0x00000001u
#define DK1_PLAYER_CALL_AFE4   0x00000002u
#define DK1_PLAYER_CALL_B012   0x00000004u
#define DK1_PLAYER_CALL_MOVE   0x00000008u
#define DK1_PLAYER_CALL_AF97   0x00000010u
#define DK1_PLAYER_CALL_GUARD  0x00000020u
#define DK1_PLAYER_CALL_INPUT  0x00000040u
#define DK1_PLAYER_CALL_COLLIDE 0x00000080u
#define DK1_PLAYER_CALL_EVENTS 0x00000100u
#define DK1_PLAYER_CALL_A4D5   0x00000200u

typedef struct Dk1PlayerStateFrame {
    uint16_t state;
    uint16_t flags_0579;
    uint16_t flags_12a5;
    uint16_t aux_flags_1271;
    uint16_t aux_flags_1699;
    uint16_t timer_1375;
    uint16_t field_0d11;
    uint16_t field_0ae5;
    uint16_t field_11a1;
    int16_t world_x;
    int16_t world_y;
    int16_t velocity_x;
    int16_t velocity_y;
    int16_t subvelocity_x;
    int16_t target_x;
    int16_t target_y;
    uint16_t game_mode;
} Dk1PlayerStateFrame;

typedef struct Dk1PlayerStateInputs {
    bool guard_aborted;
    bool gate_carry;
    bool collision_carry;
    int16_t post_move_x;
} Dk1PlayerStateInputs;

typedef struct Dk1PlayerStateResult {
    uint32_t required_calls;
    uint16_t input_mode;
    uint16_t effect_id;
    bool translated;
    bool finished;
} Dk1PlayerStateResult;

bool dk1_player_state_step_compact(Dk1PlayerStateFrame *frame,
                                   Dk1PlayerStateInputs inputs,
                                   Dk1PlayerStateResult *result);
bool dk1_player_state_compact_is_translated(uint16_t state);

#endif
