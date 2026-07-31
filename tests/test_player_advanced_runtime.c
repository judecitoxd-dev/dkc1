#include <assert.h>
#include <string.h>
#include "dk1/player_advanced_runtime.h"

int main(void) {
    Dk1PlayerAdvancedFrame f;
    Dk1PlayerAdvancedInputs in;
    Dk1PlayerAdvancedResult r;
    uint16_t state;

    for (state = 35u; state <= 49u; ++state) assert(dk1_player_state_advanced_is_translated(state));
    assert(!dk1_player_state_advanced_is_translated(34u));
    assert(!dk1_player_state_advanced_is_translated(50u));

    memset(&f, 0, sizeof(f)); memset(&in, 0, sizeof(in));
    f.state = 35u; f.timer_1375 = 2u; f.velocity_x = 320; f.accel_x = 32;
    f.velocity_y = -320; f.accel_y = -32; f.source_x = 100; f.source_y = 200;
    f.field_145d = -0x05F0; f.field_13e9 = 10; f.dp20 = 1; f.dp22 = 2;
    assert(dk1_player_state_step_advanced(&f, in, &r));
    assert(f.world_x == 111);
    assert(f.field_145d == (int16_t)-0x0600); assert(f.field_13e9 < 0);
    assert(f.field_0e21 == 1); assert(f.world_y == 191);

    memset(&f, 0, sizeof(f)); f.state = 37u; f.partner.present = true; f.partner.type = 0x30u;
    f.partner.world_x = 222; f.partner.field_14c5 = 10; f.partner.field_152d = 2; f.partner.velocity_x = 1;
    f.velocity_y = 30; f.accel_x = 40;
    assert(dk1_player_state_step_advanced(&f, in, &r));
    assert(f.world_x == 222); assert(f.velocity_y == 30); assert(f.accel_x == 0);
    assert(f.field_13e9 == 40); assert(f.global_1a69 == 0x50u); assert(r.input_mode == 9u);

    memset(&f, 0, sizeof(f)); f.state = 41u; f.frame_0d11 = 9u; f.flags_12a5 = 0xFFFFu;
    f.owner_flags_1699 = 0x0080u; f.partner.present = true; f.partner.frame_0d11 = 4u;
    assert(dk1_player_state_step_advanced(&f, in, &r));
    assert(f.frame_0d11 == 0u && f.partner.frame_0d11 == 0u);
    assert(f.flags_12a5 == 0u && f.field_1631 == 1u);
    assert((f.owner_flags_1699 & 0x0200u) != 0u && (f.owner_flags_1699 & 0x0080u) == 0u);

    memset(&f, 0, sizeof(f)); f.state = 44u; f.field_145d = 0; f.velocity_y = -0x2F0;
    assert(dk1_player_state_step_advanced(&f, in, &r));
    assert(f.velocity_y == (int16_t)-0x300);

    memset(&f, 0, sizeof(f)); f.state = 48u; f.owner_field_16ad = 0; f.accel_x = (int16_t)0x8000;
    in.bba58d_carry = true; in.bba58d_index = 7u;
    assert(dk1_player_state_step_advanced(&f, in, &r));
    assert(f.state == 0x4Cu); assert(f.velocity_y == 0); assert(r.animation_id == 0x3Au);
    assert((f.flags_0c69 & 0x4000u) != 0u);

    memset(&f, 0, sizeof(f)); memset(&in, 0, sizeof(in));
    f.state = 49u; f.world_x = 300; f.camera_x = 0; f.timer_1375 = 100u; f.game_mode = 7u;
    f.partner.present = true; f.other_player.present = true; f.other_player.state = 0x32u;
    assert(dk1_player_state_step_advanced(&f, in, &r));
    assert(f.state == 0u && f.velocity_y == (int16_t)0xFE0C && f.global_1929 == 0u);
    assert((f.flags_0c69 & 0x2000u) != 0u && (f.partner.flags_0c69 & 0x2000u) != 0u);
    assert(f.other_player.velocity_y == (int16_t)0xFE0C);
    return 0;
}
