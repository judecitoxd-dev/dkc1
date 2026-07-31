#include "dk1/player_coverage.h"
#include "dk1/player_dispatch.h"
#include "dk1/player_state_runtime.h"
Dk1PlayerTranslationLevel dk1_player_state_translation_level(uint16_t s){if(dk1_player_state_compact_is_translated(s))return DK1_PLAYER_STATE_LOCAL_SEMANTICS;if(s<2u)return DK1_PLAYER_STATE_PLAN_ONLY;return DK1_PLAYER_STATE_UNTRANSLATED;}
size_t dk1_player_translated_state_count(void){size_t i,n=0;for(i=0;i<DK1_PLAYER_STATE_COUNT;i++)if(dk1_player_state_translation_level((uint16_t)i)==DK1_PLAYER_STATE_LOCAL_SEMANTICS)n++;return n;}
size_t dk1_player_planned_state_count(void){size_t i,n=0;for(i=0;i<DK1_PLAYER_STATE_COUNT;i++)if(dk1_player_state_translation_level((uint16_t)i)!=DK1_PLAYER_STATE_UNTRANSLATED)n++;return n;}
uint64_t dk1_player_translation_signature(void){uint64_t h=14695981039346656037ull;uint16_t s;for(s=0;s<DK1_PLAYER_STATE_COUNT;s++){uint32_t pc=dk1_player_state_handler_pc(s);uint8_t level=(uint8_t)dk1_player_state_translation_level(s);size_t i;for(i=0;i<4;i++){h^=(uint8_t)(pc>>(i*8));h*=1099511628211ull;}h^=level;h*=1099511628211ull;}return h;}
