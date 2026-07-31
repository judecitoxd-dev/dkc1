#include <assert.h>
#include <string.h>
#include "dk1/player_interrupt.h"
int main(void){Dk1PlayerInterruptState s;Dk1PlayerInterruptResult r;memset(&s,0,sizeof(s));assert(dk1_player_interrupt_apply(&s,&r)&&!r.abort_state);s.request_1595=0x40;assert(dk1_player_interrupt_apply(&s,&r));assert(r.abort_state&&s.state_1029==0x19&&s.field_11a1==0xC0&&r.effect_id==0x4E&&r.call_a29b);memset(&s,0,sizeof(s));s.request_1595=0x80;s.attached_object_0512=2;assert(dk1_player_interrupt_apply(&s,&r));assert(s.state_1029==0x15&&r.spawn_type==0x36);memset(&s,0,sizeof(s));s.request_1595=0x80;s.aux_16f5=1;assert(dk1_player_interrupt_apply(&s,&r));assert(s.state_1029==0x1A&&r.effect_id==0x53);return 0;}
