#ifndef DK1_PLAYER_EXTENDED_RUNTIME_H
#define DK1_PLAYER_EXTENDED_RUNTIME_H

#include <stdbool.h>
#include <stdint.h>

#define DK1_PLAYER_EXT_CALL_GUARD          (1ull << 0)
#define DK1_PLAYER_EXT_CALL_INPUT          (1ull << 1)
#define DK1_PLAYER_EXT_CALL_MOVE           (1ull << 2)
#define DK1_PLAYER_EXT_CALL_EVENTS         (1ull << 3)
#define DK1_PLAYER_EXT_CALL_ANIMATION      (1ull << 4)
#define DK1_PLAYER_EXT_CALL_STATE20_HELPER (1ull << 5)
#define DK1_PLAYER_EXT_CALL_A75B           (1ull << 6)
#define DK1_PLAYER_EXT_CALL_A110           (1ull << 7)
#define DK1_PLAYER_EXT_CALL_A5BE           (1ull << 8)
#define DK1_PLAYER_EXT_CALL_A51E           (1ull << 9)
#define DK1_PLAYER_EXT_CALL_8953           (1ull << 10)
#define DK1_PLAYER_EXT_CALL_92F9           (1ull << 11)
#define DK1_PLAYER_EXT_CALL_92D9           (1ull << 12)
#define DK1_PLAYER_EXT_CALL_A29B           (1ull << 13)
#define DK1_PLAYER_EXT_CALL_857B           (1ull << 14)
#define DK1_PLAYER_EXT_CALL_B884B6         (1ull << 15)
#define DK1_PLAYER_EXT_CALL_A599           (1ull << 16)
#define DK1_PLAYER_EXT_CALL_OBJECT_EFFECT  (1ull << 17)

typedef struct Dk1PlayerExtendedFrame {
    uint16_t state;
    uint16_t flags_12a5;
    uint16_t global_1929;
    uint16_t field_123d;
    uint16_t field_11a1;
    uint16_t facing_145d;
    uint16_t global_051a;
    int16_t world_x;
    int16_t camera_x;
    int16_t velocity_x;
    int16_t velocity_y;
    int16_t subvelocity_x;
} Dk1PlayerExtendedFrame;

typedef struct Dk1PlayerExtendedInputs {
    bool guard_aborted;
} Dk1PlayerExtendedInputs;

typedef struct Dk1PlayerExtendedResult {
    uint64_t required_calls;
    uint16_t input_mode;
    uint16_t effect_id;
    bool translated;
    bool finished;
} Dk1PlayerExtendedResult;

/* State 20 is preserved as an exact plan because its linked-object interaction is not fully translated. */
bool dk1_player_state20_plan(Dk1PlayerExtendedResult *result);

/* Executable local semantics for states 21-34. External callees remain explicit flags. */
bool dk1_player_state_step_extended(Dk1PlayerExtendedFrame *frame,
                                    Dk1PlayerExtendedInputs inputs,
                                    Dk1PlayerExtendedResult *result);
bool dk1_player_state_extended_is_translated(uint16_t state);

#endif
