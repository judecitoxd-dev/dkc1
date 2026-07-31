#ifndef DK1_PLAYER_ADVANCED_RUNTIME_H
#define DK1_PLAYER_ADVANCED_RUNTIME_H

#include <stdbool.h>
#include <stdint.h>

#define DK1_PLAYER_ADV_CALL_GUARD       (1ull << 0)
#define DK1_PLAYER_ADV_CALL_INPUT       (1ull << 1)
#define DK1_PLAYER_ADV_CALL_MOVE        (1ull << 2)
#define DK1_PLAYER_ADV_CALL_EVENTS      (1ull << 3)
#define DK1_PLAYER_ADV_CALL_ANIMATION   (1ull << 4)
#define DK1_PLAYER_ADV_CALL_SET_ANIM    (1ull << 5)
#define DK1_PLAYER_ADV_CALL_857B        (1ull << 6)
#define DK1_PLAYER_ADV_CALL_8589        (1ull << 7)
#define DK1_PLAYER_ADV_CALL_858C        (1ull << 8)
#define DK1_PLAYER_ADV_CALL_8590        (1ull << 9)
#define DK1_PLAYER_ADV_CALL_8663        (1ull << 10)
#define DK1_PLAYER_ADV_CALL_8672        (1ull << 11)
#define DK1_PLAYER_ADV_CALL_871C        (1ull << 12)
#define DK1_PLAYER_ADV_CALL_8778        (1ull << 13)
#define DK1_PLAYER_ADV_CALL_8953        (1ull << 14)
#define DK1_PLAYER_ADV_CALL_90CB        (1ull << 15)
#define DK1_PLAYER_ADV_CALL_9382        (1ull << 16)
#define DK1_PLAYER_ADV_CALL_A067        (1ull << 17)
#define DK1_PLAYER_ADV_CALL_A07C        (1ull << 18)
#define DK1_PLAYER_ADV_CALL_A0D0        (1ull << 19)
#define DK1_PLAYER_ADV_CALL_A0F9        (1ull << 20)
#define DK1_PLAYER_ADV_CALL_A3CA        (1ull << 21)
#define DK1_PLAYER_ADV_CALL_A485        (1ull << 22)
#define DK1_PLAYER_ADV_CALL_A55F        (1ull << 23)
#define DK1_PLAYER_ADV_CALL_A6E0        (1ull << 24)
#define DK1_PLAYER_ADV_CALL_A75B        (1ull << 25)
#define DK1_PLAYER_ADV_CALL_AFB2        (1ull << 26)
#define DK1_PLAYER_ADV_CALL_AF81        (1ull << 27)
#define DK1_PLAYER_ADV_CALL_B0BE        (1ull << 28)
#define DK1_PLAYER_ADV_CALL_B1D5        (1ull << 29)
#define DK1_PLAYER_ADV_CALL_B1DA        (1ull << 30)
#define DK1_PLAYER_ADV_CALL_B22E        (1ull << 31)
#define DK1_PLAYER_ADV_CALL_B268        (1ull << 32)
#define DK1_PLAYER_ADV_CALL_BBA4C8      (1ull << 33)
#define DK1_PLAYER_ADV_CALL_BBA58D      (1ull << 34)
#define DK1_PLAYER_ADV_CALL_BCB882      (1ull << 35)
#define DK1_PLAYER_ADV_CALL_BF8589      (1ull << 36)
#define DK1_PLAYER_ADV_CALL_BF858C      (1ull << 37)
#define DK1_PLAYER_ADV_CALL_BF8590      (1ull << 38)
#define DK1_PLAYER_ADV_CALL_BF8672      (1ull << 39)
#define DK1_PLAYER_ADV_CALL_BFB1D5      (1ull << 40)
#define DK1_PLAYER_ADV_CALL_BFB1DA      (1ull << 41)
#define DK1_PLAYER_ADV_CALL_FBE7        (1ull << 42)
#define DK1_PLAYER_ADV_CALL_8827        (1ull << 43)
#define DK1_PLAYER_ADV_CALL_EFFECT      (1ull << 44)

/* Portable view of the linked player/object fields touched by states 35-49. */
typedef struct Dk1PlayerAdvancedLink {
    bool present;
    uint16_t type;
    uint16_t state;
    uint16_t flags_0c69;
    uint16_t flags_1699;
    uint16_t field_152d;
    uint16_t field_16ad;
    uint16_t field_0512;
    uint16_t frame_0d11;
    int16_t world_x;
    int16_t world_y;
    int16_t velocity_x;
    int16_t velocity_y;
    int16_t field_14c5;
    int16_t field_0b8d;
    int16_t field_13e9;
    int16_t field_145d;
    int16_t animation_speed;
    uint16_t field_0ae5;
    uint16_t field_1631;
} Dk1PlayerAdvancedLink;

typedef struct Dk1PlayerAdvancedFrame {
    uint16_t state;
    uint16_t flags_0579;
    uint16_t flags_0c69;
    uint16_t flags_12a5;
    uint16_t flags_1271;
    uint16_t timer_1375;
    uint16_t frame_0d11;
    uint16_t field_0ae5;
    uint16_t field_11a1;
    uint16_t field_11d5;
    uint16_t field_123d;
    uint16_t field_1631;
    uint16_t global_1929;
    uint16_t global_051a;
    uint16_t global_1a69;
    uint16_t global_1811;
    uint16_t global_1813;
    uint16_t game_mode;
    uint16_t frame_counter;
    uint16_t owner_flags_1699;
    uint16_t owner_field_16e5;
    uint16_t owner_field_16ad;
    int16_t world_x;
    int16_t world_y;
    int16_t camera_x;
    int16_t camera_y;
    int16_t global_y_4a;
    int16_t velocity_x;
    int16_t velocity_y;
    int16_t accel_x;
    int16_t accel_y;
    int16_t field_0e21;
    int16_t field_0b8d;
    int16_t field_13e9;
    int16_t field_145d;
    int16_t source_x;
    int16_t source_y;
    int16_t dp20;
    int16_t dp22;
    int16_t dp24;
    int16_t dp26;
    int16_t animation_speed;
    Dk1PlayerAdvancedLink partner;
    Dk1PlayerAdvancedLink other_player;
} Dk1PlayerAdvancedFrame;

typedef struct Dk1PlayerAdvancedInputs {
    bool guard_aborted;
    bool a3ca_aborted;
    bool a55f_carry;
    bool bba58d_carry;
    uint16_t bba58d_index;
} Dk1PlayerAdvancedInputs;

typedef struct Dk1PlayerAdvancedResult {
    uint64_t required_calls;
    uint16_t input_mode;
    uint16_t input_argument;
    uint16_t animation_id;
    uint16_t effect_id;
    bool translated;
    bool finished;
} Dk1PlayerAdvancedResult;

bool dk1_player_state_advanced_is_translated(uint16_t state);
bool dk1_player_state_step_advanced(Dk1PlayerAdvancedFrame *frame,
                                    Dk1PlayerAdvancedInputs inputs,
                                    Dk1PlayerAdvancedResult *result);

#endif
