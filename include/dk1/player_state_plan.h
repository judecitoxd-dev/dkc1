#ifndef DK1_PLAYER_STATE_PLAN_H
#define DK1_PLAYER_STATE_PLAN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum Dk1PlayerPlanCallKind {
    DK1_PLAYER_PLAN_JSR_BF = 0,
    DK1_PLAYER_PLAN_JSL,
    DK1_PLAYER_PLAN_CLEAR_FIELD
} Dk1PlayerPlanCallKind;

typedef struct Dk1PlayerPlanStep {
    Dk1PlayerPlanCallKind kind;
    uint32_t source_pc;
    uint16_t argument;
    bool may_abort_state;
} Dk1PlayerPlanStep;

typedef struct Dk1PlayerStatePlan {
    uint16_t state;
    uint16_t handler_bf;
    const Dk1PlayerPlanStep *steps;
    size_t step_count;
} Dk1PlayerStatePlan;

bool dk1_player_state_plan_get(uint16_t state, Dk1PlayerStatePlan *plan);

#endif
