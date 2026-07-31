#ifndef DK1_SAVE_STATE_H
#define DK1_SAVE_STATE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/scene_runtime.h"

#define DK1_SAVE_STATE_BYTES 48u

typedef struct Dk1SaveStateInfo {
    uint16_t level_id;
    uint16_t flags;
    uint64_t scene_signature;
    Dk1SceneRuntime runtime;
} Dk1SaveStateInfo;

bool dk1_save_state_encode(const Dk1SaveStateInfo *state,
                           uint8_t output[DK1_SAVE_STATE_BYTES]);
bool dk1_save_state_decode(const uint8_t input[DK1_SAVE_STATE_BYTES],
                           Dk1SaveStateInfo *state);
bool dk1_save_state_write_file(const char *path, const Dk1SaveStateInfo *state);
bool dk1_save_state_read_file(const char *path, Dk1SaveStateInfo *state);

#endif
