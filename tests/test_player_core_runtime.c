#include <assert.h>
#include <string.h>
#include "dk1/player_core_runtime.h"

static Dk1PlayerCoreFrame make_frame(uint16_t state) {
    Dk1PlayerCoreFrame f;
    memset(&f, 0, sizeof(f));
    f.current.state = state;
    return f;
}

int main(void) {
    Dk1PlayerCoreFrame f;
    Dk1PlayerCoreInputs in;
    Dk1PlayerCoreResult r;
    memset(&in, 0, sizeof(in));

    f = make_frame(2u);
    f.other.state = 0x26u;
    f.current.world_x = 100;
    f.other.world_x = 120;
    assert(dk1_player_state_step_core(&f, in, &r));
    assert(f.current.state == 0x33u && r.finished);

    f = make_frame(2u);
    f.other.state = 0x29u;
    f.current.world_x = 100; f.current.world_y = 80;
    f.other.world_x = 110; f.other.world_y = 84;
    assert(dk1_player_state_step_core(&f, in, &r));
    assert(f.current.state == 0x35u && f.current.world_x == 110 &&
           (r.required_calls & DK1_PLAYER_CORE_CALL_BCB882) != 0u);

    f = make_frame(3u);
    f.current.field_1375 = 1u;
    f.current.field_0b8d = 10;
    f.other.field_0b8d = 20;
    f.owner.anchor_x_16b9 = 300;
    f.owner.anchor_y_16bd = 70;
    f.owner.flags_1699 = 0xFFFFu;
    assert(dk1_player_state_step_core(&f, in, &r));
    assert(f.current.state == 0u && f.current.world_x == 300 && f.current.world_y == 70);
    assert(f.current.field_0b8d == 20 && f.other.field_0b8d == 10);
    assert((f.owner.flags_1699 & 8u) == 0u && r.finished);

    f = make_frame(4u);
    f.current.field_1375 = 2u;
    f.current.velocity_y = -1;
    f.current.world_y = 30;
    f.owner.anchor_y_16bd = 40;
    assert(dk1_player_state_step_core(&f, in, &r));
    assert(f.current.world_y == 40);
    assert((r.required_calls & DK1_PLAYER_CORE_CALL_ANIMATION) != 0u);

    f = make_frame(5u);
    f.current.world_x = 100;
    f.owner.anchor_x_16b9 = 110;
    f.global_1927 = 2u;
    f.global_1813 = 3u;
    assert(dk1_player_state_step_core(&f, in, &r));
    assert(f.current.velocity_x == 160 && f.current.state == 6u && r.finished);

    f = make_frame(10u);
    f.owner.anchor_x_16b9 = 123;
    f.owner.anchor_y_16bd = 45;
    f.owner.launch_x_16c1 = 0x120;
    f.current.field_0b8d = 1;
    f.other.field_0b8d = 2;
    in.external_result_is_one = true;
    assert(dk1_player_state_step_core(&f, in, &r));
    assert(f.current.state == 0x53u && f.current.world_x == 123 && f.current.world_y == 45);
    assert(f.current.velocity_x == 0x120 && f.current.velocity_y == 0x0A00);
    assert(f.global_1929 == 3u && r.animation_id == 5u);

    f = make_frame(10u);
    f.game_mode = 3u;
    f.owner.type_16ad = 0x0Fu;
    in.external_result_is_one = false;
    assert(dk1_player_state_step_core(&f, in, &r));
    assert(f.current.state == 0x2Bu && f.current.velocity_y == 0x80 && r.animation_id == 0x60u);

    assert(dk1_player_state_core_is_translated(2u));
    assert(dk1_player_state_core_is_translated(10u));
    assert(!dk1_player_state_core_is_translated(6u));
    return 0;
}
