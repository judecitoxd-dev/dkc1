#include <assert.h>
#include <string.h>
#include "dk1/object_scheduler.h"

typedef struct VisitLog {
    size_t count;
    size_t slots[64];
    Dk1ObjectPass passes[64];
} VisitLog;

static void log_visit(
    size_t slot,
    Dk1ObjectPass pass,
    uint32_t callback_pc,
    void *data
) {
    VisitLog *log = data;
    assert((callback_pc >> 16u) == 0xBFu);
    log->slots[log->count] = slot;
    log->passes[log->count] = pass;
    ++log->count;
}

static Dk1ObjectFrameState base_state(void) {
    Dk1ObjectFrameState state;
    memset(&state, 0, sizeof(state));
    state.render_phase_1afb = 0x12ABu;
    state.global_flags_0579 = 0xFFFFu;
    state.countdown_0535 = 2u;
    return state;
}

int main(void) {
    Dk1ObjectFrameState state = base_state();
    Dk1ObjectFrameResult result;
    VisitLog log = {0};

    state.type_id[1] = 2u;
    state.type_id[3] = 1u;
    state.type_id[26] = 2u;
    result = dk1_object_run_frame(&state, log_visit, &log);
    assert(state.render_phase_1afb == 0xAB00u);
    assert(state.global_flags_0579 == 0xEFFFu);
    assert(state.countdown_0535 == 1u);
    assert(result.secondary_callbacks == 1u);
    assert(result.primary_callbacks == 2u);
    assert(log.slots[0] == 26u);
    assert(log.passes[0] == DK1_OBJECT_PASS_SECONDARY);
    assert(log.slots[1] == 1u && log.slots[2] == 3u);

    state = base_state();
    log = (VisitLog){0};
    state.frame_mode_1929 = 0xFFFFu;
    state.type_id[1] = 2u;
    state.type_id[26] = 2u;
    result = dk1_object_run_frame(&state, log_visit, &log);
    assert(result.secondary_callbacks == 1u);
    assert(result.primary_callbacks == 0u);
    assert(result.primary_pass_skipped);
    assert(!result.post_update_requested);
    assert(state.frame_mode_1929 == 0u);

    state = base_state();
    log = (VisitLog){0};
    state.frame_mode_1929 = 0x0002u;
    state.type_id[1] = 2u;
    state.type_id[3] = 0x31u;
    state.type_id[4] = 0x17u;
    state.type_id[5] = 0x26u;
    state.state_1029[5] = 0u;
    state.type_id[6] = 0x26u;
    state.state_1029[6] = 3u;
    state.type_id[7] = 2u;
    result = dk1_object_run_frame(&state, log_visit, &log);
    assert(result.primary_callbacks == 4u);

    state = base_state();
    log = (VisitLog){0};
    state.frame_mode_1929 = 0x0010u;
    state.focus_offset_1923 = 8u;
    state.type_id[3] = 0x45u;
    state.type_id[4] = 2u;
    state.type_id[5] = 2u;
    result = dk1_object_run_frame(&state, log_visit, &log);
    assert(result.primary_callbacks == 2u);
    assert(log.slots[0] == 3u && log.slots[1] == 4u);

    state = base_state();
    log = (VisitLog){0};
    state.frame_mode_1929 = 0x0008u;
    state.focus_offset_1923 = 2u;
    state.type_id[1] = 1u;
    state.type_id[2] = 1u;
    state.type_id[3] = 2u;
    result = dk1_object_run_frame(&state, log_visit, &log);
    assert(result.primary_callbacks == 2u);
    assert(log.slots[0] == 2u && log.slots[1] == 3u);
    assert(log.passes[0] == DK1_OBJECT_PASS_PRIMARY_SPECIAL);
    assert(log.passes[1] == DK1_OBJECT_PASS_PRIMARY_SPECIAL);

    return 0;
}
