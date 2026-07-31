#ifndef DK1_PLAYER_LATE_RUNTIME_H
#define DK1_PLAYER_LATE_RUNTIME_H

#include <stdbool.h>
#include <stdint.h>

#define DK1_PLAYER_LATE_CALL_ANIMATION        (1ull << 0)
#define DK1_PLAYER_LATE_CALL_REVERSE_ANIM     (1ull << 1)
#define DK1_PLAYER_LATE_CALL_SET_ANIM         (1ull << 2)
#define DK1_PLAYER_LATE_CALL_B0BE             (1ull << 3)
#define DK1_PLAYER_LATE_CALL_BCB882           (1ull << 4)
#define DK1_PLAYER_LATE_CALL_BCB874           (1ull << 5)
#define DK1_PLAYER_LATE_CALL_A9FC             (1ull << 6)
#define DK1_PLAYER_LATE_CALL_A067             (1ull << 7)
#define DK1_PLAYER_LATE_CALL_C55A             (1ull << 8)
#define DK1_PLAYER_LATE_CALL_8CC1             (1ull << 9)
#define DK1_PLAYER_LATE_CALL_9B49             (1ull << 10)
#define DK1_PLAYER_LATE_CALL_9B22             (1ull << 11)
#define DK1_PLAYER_LATE_CALL_867B             (1ull << 12)
#define DK1_PLAYER_LATE_CALL_8663             (1ull << 13)
#define DK1_PLAYER_LATE_CALL_80812F           (1ull << 14)
#define DK1_PLAYER_LATE_CALL_B6A87A           (1ull << 15)
#define DK1_PLAYER_LATE_CALL_92F9             (1ull << 16)
#define DK1_PLAYER_LATE_CALL_BE8092           (1ull << 17)
#define DK1_PLAYER_LATE_CALL_928A             (1ull << 18)
#define DK1_PLAYER_LATE_CALL_929C             (1ull << 19)
#define DK1_PLAYER_LATE_CALL_GUARD            (1ull << 20)
#define DK1_PLAYER_LATE_CALL_INPUT            (1ull << 21)
#define DK1_PLAYER_LATE_CALL_BF8589           (1ull << 22)
#define DK1_PLAYER_LATE_CALL_A75B             (1ull << 23)
#define DK1_PLAYER_LATE_CALL_8827             (1ull << 24)
#define DK1_PLAYER_LATE_CALL_9926             (1ull << 25)
#define DK1_PLAYER_LATE_CALL_AACB             (1ull << 26)
#define DK1_PLAYER_LATE_CALL_FBE7             (1ull << 27)
#define DK1_PLAYER_LATE_CALL_EFFECT           (1ull << 28)
#define DK1_PLAYER_LATE_CALL_BDF7F2           (1ull << 29)

typedef struct Dk1PlayerLateActor {
    uint16_t state;
    uint16_t flags_0c69;
    uint16_t flags_1271;
    uint16_t flags_12a5;
    uint16_t frame_0d11;
    uint16_t cached_frame_0ae5;
    uint16_t field_11a1;
    uint16_t field_123d;
    uint16_t field_1375;
    uint16_t field_145d;
    uint16_t field_1631;
    int16_t field_0b8d;
    int16_t world_x;
    int16_t world_y;
    int16_t velocity_x;
    int16_t velocity_y;
    int16_t target_velocity_x;
} Dk1PlayerLateActor;

typedef struct Dk1PlayerLateOwner {
    uint16_t type_16ad;
    uint16_t flags_1699;
    uint16_t link_0512;
} Dk1PlayerLateOwner;

typedef struct Dk1PlayerLateFrame {
    Dk1PlayerLateActor current;
    Dk1PlayerLateActor other;
    Dk1PlayerLateOwner owner;
    uint16_t current_slot_offset;
    uint16_t global_flags_0579;
    uint16_t global_flags_0c69;
    uint16_t global_051a;
    uint16_t global_1811;
    uint16_t global_1813;
    uint16_t global_1917;
    uint16_t global_1929;
    uint16_t global_1a69;
    uint16_t game_mode;
    int16_t camera_x;
} Dk1PlayerLateFrame;

typedef struct Dk1PlayerLateInputs {
    bool guard_aborted;
    bool bdf7f2_carry;
} Dk1PlayerLateInputs;

typedef struct Dk1PlayerLateResult {
    uint64_t required_calls;
    uint16_t input_mode;
    uint16_t animation_id;
    uint16_t other_animation_id;
    uint16_t effect_id;
    bool translated;
    bool finished;
} Dk1PlayerLateResult;

bool dk1_player_state_late_is_translated(uint16_t state);
bool dk1_player_state_step_late(Dk1PlayerLateFrame *frame,
                                Dk1PlayerLateInputs inputs,
                                Dk1PlayerLateResult *result);

#endif
