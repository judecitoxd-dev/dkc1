#include <assert.h>
#include "dk1/player_events.h"
int main(void) {
    Dk1PlayerEventResult r;
    r=dk1_player_event_transition(4u,0u,0u);
    assert(r.consumed && r.new_state==0x4Eu && r.global_051a==0x820Fu);
    r=dk1_player_event_transition(3u,1u,0u);
    assert(r.consumed && r.new_state==0x55u && r.call_fd8c && r.call_a29b);
    r=dk1_player_event_transition(1u,0u,0u); assert(!r.consumed);
    r=dk1_player_event_transition(1u,1u,0x20u);
    assert(r.consumed && r.new_state==0x11u && !r.call_a29b);
    r=dk1_player_event_transition(2u,0x1001u,0u); assert(!r.consumed);
    r=dk1_player_event_transition(2u,1u,0u);
    assert(r.consumed && r.new_state==0x11u && r.call_fda6);
    return 0;
}
