#include <assert.h>
#include "dk1/player_coverage.h"

int main(void) {
    assert(dk1_player_translated_state_count() == 79u);
    assert(dk1_player_planned_state_count() == 82u);
    assert(dk1_player_state_translation_level(0u) == DK1_PLAYER_STATE_PLAN_ONLY);
    assert(dk1_player_state_translation_level(6u) == DK1_PLAYER_STATE_LOCAL_SEMANTICS);
    assert(dk1_player_state_translation_level(20u) == DK1_PLAYER_STATE_PLAN_ONLY);
    assert(dk1_player_state_translation_level(50u) == DK1_PLAYER_STATE_LOCAL_SEMANTICS);
    assert(dk1_player_state_translation_level(71u) == DK1_PLAYER_STATE_LOCAL_SEMANTICS);
    assert(dk1_player_state_translation_level(86u) == DK1_PLAYER_STATE_LOCAL_SEMANTICS);
    assert(dk1_player_state_translation_level(2u) == DK1_PLAYER_STATE_UNTRANSLATED);
    assert(dk1_player_translation_signature() == 0x86F04E4A511B0A0Bull);
    return 0;
}
