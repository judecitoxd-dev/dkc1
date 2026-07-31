#ifndef DK1_OBJECT_SCHEDULER_H
#define DK1_OBJECT_SCHEDULER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/object_runtime.h"

typedef enum Dk1ObjectPass {
    DK1_OBJECT_PASS_SECONDARY = 0,
    DK1_OBJECT_PASS_PRIMARY_NORMAL,
    DK1_OBJECT_PASS_PRIMARY_RESTRICTED,
    DK1_OBJECT_PASS_PRIMARY_FOCUS,
    DK1_OBJECT_PASS_PRIMARY_SPECIAL,
} Dk1ObjectPass;

typedef struct Dk1ObjectFrameState {
    uint16_t render_phase_1afb;
    uint16_t global_flags_0579;
    uint16_t frame_mode_1929;
    uint16_t focus_offset_1923;
    uint16_t countdown_0535;
    uint16_t type_id[DK1_OBJECT_SLOT_COUNT];
    uint16_t state_1029[DK1_OBJECT_SLOT_COUNT];
} Dk1ObjectFrameState;

typedef struct Dk1ObjectFrameResult {
    size_t secondary_callbacks;
    size_t primary_callbacks;
    bool primary_pass_skipped;
    bool post_update_requested;
} Dk1ObjectFrameResult;

typedef void (*Dk1ObjectVisitFn)(
    size_t slot,
    Dk1ObjectPass pass,
    uint32_t callback_pc,
    void *user_data
);

/*
 * Semantic model of $BF:8000-$BF:80F4 and $BF:815E-$BF:8175.
 * Callback targets are bank-$BF handlers from the table at $BF:817C.
 */
Dk1ObjectFrameResult dk1_object_run_frame(
    Dk1ObjectFrameState *state,
    Dk1ObjectVisitFn visit,
    void *user_data
);

#endif
