#include "dk1/player_coverage.h"
#include "dk1/player_dispatch.h"
#include "dk1/player_advanced_runtime.h"
#include "dk1/player_extended_runtime.h"
#include "dk1/player_late_runtime.h"
#include "dk1/player_state_runtime.h"

Dk1PlayerTranslationLevel dk1_player_state_translation_level(uint16_t state) {
    if (dk1_player_state_compact_is_translated(state) ||
        dk1_player_state_extended_is_translated(state) ||
        dk1_player_state_advanced_is_translated(state) ||
        dk1_player_state_late_is_translated(state))
        return DK1_PLAYER_STATE_LOCAL_SEMANTICS;
    if (state < 2u || state == 20u)
        return DK1_PLAYER_STATE_PLAN_ONLY;
    return DK1_PLAYER_STATE_UNTRANSLATED;
}

size_t dk1_player_translated_state_count(void) {
    size_t i, count = 0u;
    for (i = 0u; i < DK1_PLAYER_STATE_COUNT; ++i)
        if (dk1_player_state_translation_level((uint16_t)i) == DK1_PLAYER_STATE_LOCAL_SEMANTICS)
            ++count;
    return count;
}

size_t dk1_player_planned_state_count(void) {
    size_t i, count = 0u;
    for (i = 0u; i < DK1_PLAYER_STATE_COUNT; ++i)
        if (dk1_player_state_translation_level((uint16_t)i) != DK1_PLAYER_STATE_UNTRANSLATED)
            ++count;
    return count;
}

uint64_t dk1_player_translation_signature(void) {
    uint64_t hash = 14695981039346656037ull;
    uint16_t state;
    for (state = 0u; state < DK1_PLAYER_STATE_COUNT; ++state) {
        uint32_t pc = dk1_player_state_handler_pc(state);
        uint8_t level = (uint8_t)dk1_player_state_translation_level(state);
        size_t i;
        for (i = 0u; i < 4u; ++i) {
            hash ^= (uint8_t)(pc >> (i * 8u));
            hash *= 1099511628211ull;
        }
        hash ^= level;
        hash *= 1099511628211ull;
    }
    return hash;
}
