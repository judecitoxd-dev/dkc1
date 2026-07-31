#include <assert.h>
#include <string.h>
#include "dk1/player_state_runtime.h"

int main(void) {
    Dk1PlayerStateFrame f;
    Dk1PlayerStateResult r;
    Dk1PlayerStateInputs in = {0};
    memset(&f,0,sizeof(f)); f.state=6; f.world_x=10; f.world_y=20; f.target_x=10; f.target_y=20; f.aux_flags_1699=0xFFFF;
    assert(dk1_player_state_step_compact(&f,in,&r)); assert(f.state==2); assert((f.aux_flags_1699&8u)==0u);

    memset(&f,0,sizeof(f)); f.state=9; f.flags_12a5=2; f.world_x=100; in.post_move_x=101;
    for (int i=0;i<8;i++) assert(dk1_player_state_step_compact(&f,in,&r));
    assert(f.state==0x54 && r.effect_id==0x68 && f.velocity_x==0);

    memset(&f,0,sizeof(f)); f.state=11; f.velocity_y=-0x5F0;
    assert(dk1_player_state_step_compact(&f,in,&r)); assert(f.velocity_y==-0x600); assert(f.flags_0579&2u);

    memset(&f,0,sizeof(f)); f.state=14; f.timer_1375=0; f.game_mode=3;
    assert(dk1_player_state_step_compact(&f,in,&r)); assert(f.state==10 && r.effect_id==0x0F);

    memset(&f,0,sizeof(f)); f.state=18; f.velocity_x=123; f.subvelocity_x=44; in.collision_carry=true;
    assert(dk1_player_state_step_compact(&f,in,&r)); assert(f.velocity_y==-1 && f.velocity_x==0 && f.subvelocity_x==0);
    assert(!dk1_player_state_step_compact(0,in,&r));
    return 0;
}
