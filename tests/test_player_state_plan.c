#include <assert.h>
#include "dk1/player_state_plan.h"
int main(void){
 Dk1PlayerStatePlan p;
 assert(dk1_player_state_plan_get(0,&p)); assert(p.handler_bf==0x8756u&&p.step_count==8u);
 assert(p.steps[1].source_pc==0xBFB2C5u&&p.steps[1].argument==0u&&p.steps[1].may_abort_state);
 assert(dk1_player_state_plan_get(1,&p)); assert(p.handler_bf==0x87FDu&&p.step_count==10u);
 assert(p.steps[1].argument==1u); assert(p.steps[3].kind==DK1_PLAYER_PLAN_CLEAR_FIELD);
 assert(!dk1_player_state_plan_get(2,&p)); return 0;
}
