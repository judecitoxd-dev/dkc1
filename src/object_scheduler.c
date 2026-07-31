#include "dk1/object_scheduler.h"

static bool active_type(const Dk1ObjectFrameState *state, size_t slot) {
    return state->type_id[slot] != 0u;
}

static bool resolve_callback(uint16_t type_id, uint32_t *callback_pc, bool *special) {
    Dk1ObjectTypeDispatch entry = {0};
    if (callback_pc == NULL || special == NULL ||
        !dk1_object_type_dispatch_get(type_id, &entry)) {
        return false;
    }
    *callback_pc = ((uint32_t)DK1_OBJECT_CALLBACK_BANK << 16u) |
                   entry.callback_address_bf;
    *special = dk1_object_type_special_pass(entry);
    return true;
}

static bool restricted_primary_selected(
    const Dk1ObjectFrameState *state,
    size_t slot
) {
    const uint16_t type_id = state->type_id[slot];
    if (slot <= 2u || type_id == 0x0031u || type_id == 0x0017u) {
        return true;
    }
    if (type_id == 0x0026u) {
        const uint16_t object_state = state->state_1029[slot];
        return object_state != 0u && object_state != 5u;
    }
    return false;
}

static void visit_slot(
    const Dk1ObjectFrameState *state,
    size_t slot,
    Dk1ObjectPass pass,
    Dk1ObjectVisitFn visit,
    void *user_data,
    size_t *counter
) {
    uint32_t callback_pc = 0u;
    bool special = false;
    if (!active_type(state, slot) ||
        !resolve_callback(state->type_id[slot], &callback_pc, &special)) {
        return;
    }
    (void)special;
    if (visit != NULL) {
        visit(slot, pass, callback_pc, user_data);
    }
    ++(*counter);
}

Dk1ObjectFrameResult dk1_object_run_frame(
    Dk1ObjectFrameState *state,
    Dk1ObjectVisitFn visit,
    void *user_data
) {
    Dk1ObjectFrameResult result = {0};
    size_t slot = 0u;

    if (state == NULL) {
        return result;
    }

    /* $BF:8000-$BF:8010. */
    state->render_phase_1afb =
        (uint16_t)((state->render_phase_1afb & 0x00FFu) << 8u);
    state->global_flags_0579 &= 0xEFFFu;

    /* $BF:815E-$BF:8175: secondary objects always run first. */
    for (slot = DK1_SECONDARY_OBJECT_FIRST_SLOT;
         slot < DK1_SECONDARY_OBJECT_FIRST_SLOT +
                    DK1_SECONDARY_OBJECT_SLOT_COUNT;
         ++slot) {
        visit_slot(state, slot, DK1_OBJECT_PASS_SECONDARY, visit, user_data,
                   &result.secondary_callbacks);
    }

    /* $BF:801B-$BF:801F: a negative mode delays the primary pass. */
    if ((int16_t)state->frame_mode_1929 < 0) {
        ++state->frame_mode_1929;
        result.primary_pass_skipped = true;
        return result;
    }

    if ((state->frame_mode_1929 & 0x001Au) == 0u) {
        for (slot = DK1_PRIMARY_OBJECT_FIRST_SLOT;
             slot < DK1_PRIMARY_OBJECT_FIRST_SLOT +
                        DK1_PRIMARY_OBJECT_SLOT_COUNT;
             ++slot) {
            visit_slot(state, slot, DK1_OBJECT_PASS_PRIMARY_NORMAL,
                       visit, user_data, &result.primary_callbacks);
        }
    } else if ((state->frame_mode_1929 & 0x0018u) == 0u) {
        for (slot = DK1_PRIMARY_OBJECT_FIRST_SLOT;
             slot < DK1_PRIMARY_OBJECT_FIRST_SLOT +
                        DK1_PRIMARY_OBJECT_SLOT_COUNT;
             ++slot) {
            if (active_type(state, slot) &&
                restricted_primary_selected(state, slot)) {
                visit_slot(state, slot, DK1_OBJECT_PASS_PRIMARY_RESTRICTED,
                           visit, user_data, &result.primary_callbacks);
            }
        }
    } else if ((state->frame_mode_1929 & 0x0008u) == 0u) {
        for (slot = DK1_PRIMARY_OBJECT_FIRST_SLOT;
             slot < DK1_PRIMARY_OBJECT_FIRST_SLOT +
                        DK1_PRIMARY_OBJECT_SLOT_COUNT;
             ++slot) {
            const uint16_t offset = (uint16_t)(slot * 2u);
            if (active_type(state, slot) &&
                (state->type_id[slot] == 0x0045u ||
                 offset == state->focus_offset_1923)) {
                visit_slot(state, slot, DK1_OBJECT_PASS_PRIMARY_FOCUS,
                           visit, user_data, &result.primary_callbacks);
            }
        }
    } else {
        for (slot = DK1_PRIMARY_OBJECT_FIRST_SLOT;
             slot < DK1_PRIMARY_OBJECT_FIRST_SLOT +
                        DK1_PRIMARY_OBJECT_SLOT_COUNT;
             ++slot) {
            Dk1ObjectTypeDispatch entry = {0};
            const uint16_t offset = (uint16_t)(slot * 2u);
            if (!active_type(state, slot) ||
                offset == state->focus_offset_1923 ||
                !dk1_object_type_dispatch_get(state->type_id[slot], &entry) ||
                !dk1_object_type_special_pass(entry)) {
                continue;
            }
            visit_slot(state, slot, DK1_OBJECT_PASS_PRIMARY_SPECIAL,
                       visit, user_data, &result.primary_callbacks);
        }
    }

    if (state->countdown_0535 == 2u) {
        --state->countdown_0535;
    }
    result.post_update_requested = true;
    return result;
}
