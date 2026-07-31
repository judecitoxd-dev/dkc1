#include <assert.h>
#include "dk1/player_common.h"
int main(void) {
    Dk1PlayerCommonState p = {0};
    p.player_index = 0u; p.state = 0u; p.velocity_x = 0x0100; p.effect_scale_1139 = 0x0180;
    assert(dk1_player_common_refresh_dispatch(&p));
    assert(p.callback_pc == 0xBF84ABu && p.state_handler_pc == 0xBF8756u);
    assert(p.camera_lead_1a69 == 0x003Eu);
    assert(dk1_player_common_camera_lead(0x0100, 0x4000u) == -6);
    p.world_x = 0x0010; p.velocity_x = -0x0100; p.subvelocity_x = 5;
    assert(dk1_player_common_clamp_horizontal(&p, 0, 0x1000));
    assert(p.velocity_x == 0 && p.subvelocity_x == 0);
    p.world_x = 0x0200; p.velocity_x = 0x0100;
    assert(!dk1_player_common_clamp_horizontal(&p, 0, 0x1000));
    assert(dk1_player_common_effect_scale(0x0060u,0,0,0x0120u)==0x0110u);
    assert(dk1_player_common_effect_scale(0x0060u,0,0,0x0100u)==0x0100u);
    assert(dk1_player_common_effect_scale(0x0021u,0x0080,0x0040,0)==0x00C0u);
    assert(dk1_player_common_effect_scale(0x0029u,0x0200,0x0300,0)==0x0300u);
    assert(dk1_player_common_effect_scale(0x0065u,0,0,0)==0x0200u);
    return 0;
}
