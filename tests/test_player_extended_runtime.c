#include <assert.h>
#include <limits.h>
#include <string.h>
#include "dk1/player_extended_runtime.h"

int main(void) {
    Dk1PlayerExtendedFrame f;
    Dk1PlayerExtendedInputs in = {false};
    Dk1PlayerExtendedResult r;
    memset(&f, 0, sizeof(f));

    assert(dk1_player_state20_plan(&r));
    assert(!r.translated && r.input_mode == 0u);
    assert((r.required_calls & DK1_PLAYER_EXT_CALL_STATE20_HELPER) != 0u);

    f.state = 21u; f.field_123d = 9u;
    assert(dk1_player_state_step_extended(&f, in, &r));
    assert(f.field_123d == 0u && r.input_mode == 12u);

    f.state = 22u;
    assert(dk1_player_state_step_extended(&f, in, &r));
    assert(r.required_calls == DK1_PLAYER_EXT_CALL_ANIMATION);

    f.state = 27u;
    assert(dk1_player_state_step_extended(&f, in, &r));
    assert(f.global_1929 == 1u && f.state == 0x1Du);
    assert(f.velocity_x == 0x200 && f.subvelocity_x == 0x200);

    f.state = 28u; f.flags_12a5 = 0u;
    assert(dk1_player_state_step_extended(&f, in, &r));
    assert(r.required_calls == DK1_PLAYER_EXT_CALL_MOVE);

    f.state = 29u; f.world_x = 0x200; f.camera_x = 0x100;
    assert(dk1_player_state_step_extended(&f, in, &r));
    assert(f.state == 0x1Eu && f.global_051a == 0x820Fu);

    f.state = 30u; f.global_051a = 0x1230u;
    assert(dk1_player_state_step_extended(&f, in, &r));
    assert((r.required_calls & DK1_PLAYER_EXT_CALL_B884B6) != 0u);

    f.state = 31u; f.facing_145d = 1u; f.field_11a1 = 7u;
    assert(dk1_player_state_step_extended(&f, in, &r));
    assert(f.velocity_x == -0x200 && f.state == 0x20u && f.field_11a1 == 0u);

    f.state = 33u; f.velocity_y = -0x04F0; f.subvelocity_x = 7;
    assert(dk1_player_state_step_extended(&f, in, &r));
    assert(f.velocity_y == -0x0500 && f.subvelocity_x == 0);

    f.state = 34u;
    assert(dk1_player_state_step_extended(&f, in, &r));
    assert(r.input_mode == 8u && (r.required_calls & DK1_PLAYER_EXT_CALL_A599));

    in.guard_aborted = true; f.state = 24u;
    assert(dk1_player_state_step_extended(&f, in, &r));
    assert(r.finished && r.required_calls == DK1_PLAYER_EXT_CALL_GUARD);

    assert(dk1_player_state_extended_is_translated(21u));
    assert(dk1_player_state_extended_is_translated(34u));
    assert(!dk1_player_state_extended_is_translated(35u));
    return 0;
}
