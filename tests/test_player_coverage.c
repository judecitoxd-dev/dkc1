#include <assert.h>
#include "dk1/player_coverage.h"

int main(void) {
    assert(dk1_player_translated_state_count() == 42u);
    assert(dk1_player_planned_state_count() == 45u);
    assert(dk1_player_state_translation_level(0u) == DK1_PLAYER_STATE_PLAN_ONLY);
    assert(dk1_player_state_translation_level(6u) == DK1_PLAYER_STATE_LOCAL_SEMANTICS);
    assert(dk1_player_state_translation_level(20u) == DK1_PLAYER_STATE_PLAN_ONLY);
    assert(dk1_player_state_translation_level(21u) == DK1_PLAYER_STATE_LOCAL_SEMANTICS);
    assert(dk1_player_state_translation_level(34u) == DK1_PLAYER_STATE_LOCAL_SEMANTICS);
    assert(dk1_player_state_translation_level(35u) == DK1_PLAYER_STATE_LOCAL_SEMANTICS);
    assert(dk1_player_state_translation_level(49u) == DK1_PLAYER_STATE_LOCAL_SEMANTICS);
    assert(dk1_player_state_translation_level(50u) == DK1_PLAYER_STATE_UNTRANSLATED);
    assert(dk1_player_translation_signature() == 0xC56431FA86A799D5ull);
    return 0;
}
