#include <assert.h>
#include "dk1/player_coverage.h"
int main(void){assert(dk1_player_translated_state_count()==13u);assert(dk1_player_planned_state_count()==15u);assert(dk1_player_state_translation_level(0)==DK1_PLAYER_STATE_PLAN_ONLY);assert(dk1_player_state_translation_level(6)==DK1_PLAYER_STATE_LOCAL_SEMANTICS);assert(dk1_player_state_translation_level(20)==DK1_PLAYER_STATE_UNTRANSLATED);assert(dk1_player_translation_signature()!=0u);return 0;}
