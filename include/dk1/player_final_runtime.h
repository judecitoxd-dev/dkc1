#ifndef DK1_PLAYER_FINAL_RUNTIME_H
#define DK1_PLAYER_FINAL_RUNTIME_H

#include <stdbool.h>
#include <stdint.h>

#define DK1_PLAYER_FINAL_CALL_GUARD          (1ull << 0)
#define DK1_PLAYER_FINAL_CALL_B0BE           (1ull << 1)
#define DK1_PLAYER_FINAL_CALL_ANIMATION      (1ull << 2)
#define DK1_PLAYER_FINAL_CALL_BE80D2         (1ull << 3)
#define DK1_PLAYER_FINAL_CALL_857B           (1ull << 4)
#define DK1_PLAYER_FINAL_CALL_B884B6         (1ull << 5)
#define DK1_PLAYER_FINAL_CALL_A3CA           (1ull << 6)
#define DK1_PLAYER_FINAL_CALL_8663           (1ull << 7)
#define DK1_PLAYER_FINAL_CALL_8666           (1ull << 8)
#define DK1_PLAYER_FINAL_CALL_B1D5           (1ull << 9)
#define DK1_PLAYER_FINAL_CALL_B22E           (1ull << 10)
#define DK1_PLAYER_FINAL_CALL_8672           (1ull << 11)
#define DK1_PLAYER_FINAL_CALL_BBA4C8         (1ull << 12)
#define DK1_PLAYER_FINAL_CALL_BBA58D         (1ull << 13)
#define DK1_PLAYER_FINAL_CALL_B5802F         (1ull << 14)
#define DK1_PLAYER_FINAL_CALL_EFFECT         (1ull << 15)
#define DK1_PLAYER_FINAL_CALL_809BAA         (1ull << 16)
#define DK1_PLAYER_FINAL_CALL_BCBABD         (1ull << 17)
#define DK1_PLAYER_FINAL_CALL_B89A81         (1ull << 18)
#define DK1_PLAYER_FINAL_CALL_929C           (1ull << 19)
#define DK1_PLAYER_FINAL_CALL_A485           (1ull << 20)
#define DK1_PLAYER_FINAL_CALL_8778           (1ull << 21)
#define DK1_PLAYER_FINAL_CALL_INPUT          (1ull << 22)
#define DK1_PLAYER_FINAL_CALL_BF8589         (1ull << 23)
#define DK1_PLAYER_FINAL_CALL_A5BE           (1ull << 24)
#define DK1_PLAYER_FINAL_CALL_A51E           (1ull << 25)
#define DK1_PLAYER_FINAL_CALL_BFFB8F         (1ull << 26)
#define DK1_PLAYER_FINAL_CALL_BDF7F2         (1ull << 27)
#define DK1_PLAYER_FINAL_CALL_BF8578         (1ull << 28)
#define DK1_PLAYER_FINAL_CALL_AACB            (1ull << 29)
#define DK1_PLAYER_FINAL_CALL_8DE8            (1ull << 30)

typedef struct Dk1PlayerFinalActor {
    uint16_t state;
    uint16_t flags_12a5;
    uint16_t frame_0d11;
    uint16_t cached_frame_0ae5;
    uint16_t field_11d5;
    uint16_t field_1375;
    uint16_t field_14c5;
    uint16_t field_145d;
    int16_t velocity_x;
    int16_t velocity_y;
    int16_t target_velocity_x;
} Dk1PlayerFinalActor;

typedef struct Dk1PlayerFinalLinked {
    bool present;
    uint16_t object_index;
    uint16_t owner_link_0512;
    uint16_t flags_1699;
    uint16_t field_16f9;
    uint16_t request_1595;
    int16_t velocity_x;
    int16_t target_velocity_x;
} Dk1PlayerFinalLinked;

typedef struct Dk1PlayerFinalFrame {
    Dk1PlayerFinalActor current;
    Dk1PlayerFinalLinked linked;
    uint16_t global_051a;
    uint16_t global_0516;
    uint16_t global_0529;
    uint16_t global_052b;
    uint16_t global_052d;
    uint16_t global_0559;
    uint16_t global_055b;
    uint16_t global_055f;
    uint16_t global_0561;
    int16_t global_0565;
    uint16_t global_0579;
    uint16_t global_057b;
    uint16_t global_1811;
    uint16_t global_1813;
    uint16_t global_1929;
    uint16_t global_1a69;
    uint16_t global_1e15;
    uint16_t global_1e37;
    uint16_t level_id;
    uint16_t dp28;
    uint16_t dp4c;
} Dk1PlayerFinalFrame;

typedef struct Dk1PlayerFinalInputs {
    bool guard_aborted;
    bool a3ca_aborted;
    bool bdf7f2_carry;
    bool collision_query_carry;
    uint16_t collision_candidate_index;
    uint16_t collision_owner_link;
    uint16_t selected_link_value;
    uint16_t level_lookup_value;
    bool post_input_state_valid;
    uint16_t post_input_state;
} Dk1PlayerFinalInputs;

typedef struct Dk1PlayerFinalResult {
    uint64_t required_calls;
    uint16_t input_mode;
    uint16_t input_argument;
    uint16_t effect_id;
    uint16_t helper_argument;
    uint16_t callback_address;
    bool translated;
    bool finished;
} Dk1PlayerFinalResult;

bool dk1_player_state_final_is_translated(uint16_t state);
bool dk1_player_state_step_final(Dk1PlayerFinalFrame *frame,
                                 Dk1PlayerFinalInputs inputs,
                                 Dk1PlayerFinalResult *result);

#endif
