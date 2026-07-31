#include <assert.h>
#include "dk1/player_input_router.h"

int main(void) {
    Dk1PlayerInputRoute r;
    assert(dk1_player_input_route(3u, 0u, 0x1234u, 0xFFFFu, &r));
    assert(r.mode_argument_180f == 3u && r.pressed == 0x1234u);
    assert(r.action_count == 5u);
    assert(r.actions[0] == DK1_PLAYER_INPUT_VERTICAL_NEUTRAL);
    assert(r.actions[1] == DK1_PLAYER_INPUT_HORIZONTAL_NEUTRAL);
    assert(r.actions[2] == DK1_PLAYER_INPUT_A_RELEASED);
    assert(r.actions[3] == DK1_PLAYER_INPUT_X_RELEASED);
    assert(r.actions[4] == DK1_PLAYER_INPUT_B_RELEASED);
    assert((r.flags_1917 & 0x0040u) == 0u);

    assert(dk1_player_input_route(0u, 0xFFFFu, 0u, 0u, &r));
    assert(r.actions[0] == DK1_PLAYER_INPUT_UP); /* up wins over down */
    assert(r.actions[1] == DK1_PLAYER_INPUT_LEFT); /* left wins over right */
    assert(r.action_count == 10u);
    assert(r.actions[9] == DK1_PLAYER_INPUT_START);
    assert(!dk1_player_input_route(0u,0u,0u,0u,0));
    return 0;
}
