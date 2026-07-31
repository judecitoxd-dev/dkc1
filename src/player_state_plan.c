#include "dk1/player_state_plan.h"

static const Dk1PlayerPlanStep STATE_0[] = {
    {DK1_PLAYER_PLAN_JSR_BF,0xBFA132u,0u,true},
    {DK1_PLAYER_PLAN_JSR_BF,0xBFB2C5u,0u,true},
    {DK1_PLAYER_PLAN_JSR_BF,0xBF8953u,0u,false},
    {DK1_PLAYER_PLAN_JSL,0xBF8589u,0u,false},
    {DK1_PLAYER_PLAN_JSR_BF,0xBFA75Bu,0u,false},
    {DK1_PLAYER_PLAN_JSR_BF,0xBFA599u,0u,false},
    {DK1_PLAYER_PLAN_JSR_BF,0xBFA4F2u,0u,false},
    {DK1_PLAYER_PLAN_JSR_BF,0xBF8778u,0u,false}
};
static const Dk1PlayerPlanStep STATE_1[] = {
    {DK1_PLAYER_PLAN_JSR_BF,0xBFA132u,0u,true},
    {DK1_PLAYER_PLAN_JSR_BF,0xBFB2C5u,1u,false},
    {DK1_PLAYER_PLAN_JSR_BF,0xBF8953u,0u,false},
    {DK1_PLAYER_PLAN_CLEAR_FIELD,0x00123Du,0u,false},
    {DK1_PLAYER_PLAN_JSL,0xBF8589u,0u,false},
    {DK1_PLAYER_PLAN_JSR_BF,0xBF8899u,0u,true},
    {DK1_PLAYER_PLAN_JSR_BF,0xBFA75Bu,0u,false},
    {DK1_PLAYER_PLAN_JSR_BF,0xBFA599u,0u,false},
    {DK1_PLAYER_PLAN_JSR_BF,0xBF8827u,0u,false},
    {DK1_PLAYER_PLAN_JSR_BF,0xBF8778u,0u,false}
};

bool dk1_player_state_plan_get(uint16_t state, Dk1PlayerStatePlan *plan) {
    if (plan == NULL) return false;
    if (state == 0u) {
        *plan=(Dk1PlayerStatePlan){0u,0x8756u,STATE_0,sizeof(STATE_0)/sizeof(STATE_0[0])};
        return true;
    }
    if (state == 1u) {
        *plan=(Dk1PlayerStatePlan){1u,0x87FDu,STATE_1,sizeof(STATE_1)/sizeof(STATE_1[0])};
        return true;
    }
    *plan=(Dk1PlayerStatePlan){0};
    return false;
}
