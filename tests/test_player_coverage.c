#include <assert.h>
#include "dk1/player_coverage.h"

int main(void) {
    assert(dk1_player_translated_state_count() == 84u);
    assert(dk1_player_planned_state_count() == 87u);
    assert(dk1_player_state_translation_level(0u) == DK1_PLAYER_STATE_PLAN_ONLY);
    assert(dk1_player_state_translation_level(1u) == DK1_PLAYER_STATE_PLAN_ONLY);
    assert(dk1_player_state_translation_level(2u) == DK1_PLAYER_STATE_LOCAL_SEMANTICS);
    assert(dk1_player_state_translation_level(5u) == DK1_PLAYER_STATE_LOCAL_SEMANTICS);
    assert(dk1_player_state_translation_level(10u) == DK1_PLAYER_STATE_LOCAL_SEMANTICS);
    assert(dk1_player_state_translation_level(20u) == DK1_PLAYER_STATE_PLAN_ONLY);
    assert(dk1_player_state_translation_level(86u) == DK1_PLAYER_STATE_LOCAL_SEMANTICS);
    assert(dk1_player_translation_signature() == 0x3267767EC866CDADull);
    return 0;
}
