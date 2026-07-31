#ifndef DK1_PLAYER_COVERAGE_H
#define DK1_PLAYER_COVERAGE_H
#include <stddef.h>
#include <stdint.h>
typedef enum Dk1PlayerTranslationLevel { DK1_PLAYER_STATE_UNTRANSLATED=0, DK1_PLAYER_STATE_PLAN_ONLY, DK1_PLAYER_STATE_LOCAL_SEMANTICS } Dk1PlayerTranslationLevel;
Dk1PlayerTranslationLevel dk1_player_state_translation_level(uint16_t state);
size_t dk1_player_translated_state_count(void);
size_t dk1_player_planned_state_count(void);
uint64_t dk1_player_translation_signature(void);
#endif
