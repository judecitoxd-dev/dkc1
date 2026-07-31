#ifndef DK1_PLAYER_CORE_RUNTIME_H
#define DK1_PLAYER_CORE_RUNTIME_H

#include <stdbool.h>
#include <stdint.h>

#define DK1_PLAYER_CORE_CALL_C486       (1ull << 0)
#define DK1_PLAYER_CORE_CALL_8CC1       (1ull << 1)
#define DK1_PLAYER_CORE_CALL_8A8C       (1ull << 2)
#define DK1_PLAYER_CORE_CALL_ANIMATION  (1ull << 3)
#define DK1_PLAYER_CORE_CALL_A8FE       (1ull << 4)
#define DK1_PLAYER_CORE_CALL_A9FC       (1ull << 5)
#define DK1_PLAYER_CORE_CALL_BCB882     (1ull << 6)
#define DK1_PLAYER_CORE_CALL_AF81       (1ull << 7)
#define DK1_PLAYER_CORE_CALL_AFE4       (1ull << 8)
#define DK1_PLAYER_CORE_CALL_B012       (1ull << 9)
#define DK1_PLAYER_CORE_CALL_C121       (1ull << 10)
#define DK1_PLAYER_CORE_CALL_PAIR_LAUNCH (1ull << 11)
#define DK1_PLAYER_CORE_CALL_A067       (1ull << 12)
#define DK1_PLAYER_CORE_CALL_AA08       (1ull << 13)
#define DK1_PLAYER_CORE_CALL_A485       (1ull << 14)
#define DK1_PLAYER_CORE_CALL_BCB83A     (1ull << 15)
#define DK1_PLAYER_CORE_CALL_B881FE     (1ull << 16)
#define DK1_PLAYER_CORE_CALL_FBE7       (1ull << 17)

/* Fields locally touched by the five large handlers 2-5 and 10. */
typedef struct Dk1PlayerCoreActor {
    uint16_t state;
    uint16_t flags_0c69;
    uint16_t flags_1271;
    uint16_t flags_12a5;
    uint16_t frame_0d11;
    uint16_t field_0ae5;
    uint16_t field_11a1;
    uint16_t field_1375;
    uint16_t field_14c5;
    uint16_t field_1631;
    int16_t field_0b8d;
    int16_t world_x;
    int16_t world_y;
    int16_t velocity_x;
    int16_t velocity_y;
    int16_t field_0db9;
    int16_t field_0e21;
    int16_t animation_speed;
} Dk1PlayerCoreActor;

typedef struct Dk1PlayerCoreOwner {
    uint16_t type_16ad;
    uint16_t flags_1699;
    uint16_t flags_16cd;
    int16_t anchor_x_16b9;
    int16_t anchor_y_16bd;
    int16_t launch_x_16c1;
    int16_t launch_y_16c5;
} Dk1PlayerCoreOwner;

typedef struct Dk1PlayerCoreFrame {
    Dk1PlayerCoreActor current;
    Dk1PlayerCoreActor other;
    Dk1PlayerCoreOwner owner;
    uint16_t global_051a;
    uint16_t global_056f;
    uint16_t global_0579;
    uint16_t global_16e5;
    uint16_t global_16e7;
    uint16_t global_1813;
    uint16_t global_1917;
    uint16_t global_1921;
    uint16_t global_1927;
    uint16_t global_1929;
    uint16_t global_1b01;
    uint16_t game_mode;
} Dk1PlayerCoreFrame;

typedef struct Dk1PlayerCoreInputs {
    bool pair_launch_velocity_valid;
    int16_t pair_launch_velocity;
    bool external_result_is_one;
    bool force_state2_branch;
} Dk1PlayerCoreInputs;

typedef struct Dk1PlayerCoreResult {
    uint64_t required_calls;
    uint16_t animation_id;
    uint16_t event_id;
    bool translated;
    bool finished;
} Dk1PlayerCoreResult;

bool dk1_player_state_core_is_translated(uint16_t state);
bool dk1_player_state_step_core(Dk1PlayerCoreFrame *frame,
                                Dk1PlayerCoreInputs inputs,
                                Dk1PlayerCoreResult *result);

#endif
