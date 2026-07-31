#include <assert.h>
#include <string.h>
#include "dk1/player_motion.h"

int main(void) {
    Dk1PlayerMotion m;
    memset(&m, 0, sizeof(m));

    m.world_x = 100;
    m.velocity_x = 0x0180;
    dk1_player_motion_integrate_horizontal(&m);
    assert(m.world_x == 101 && m.subpixel_x == 0x8000u);
    dk1_player_motion_integrate_horizontal(&m);
    assert(m.world_x == 103 && m.subpixel_x == 0u);

    m.world_x = 100;
    m.subpixel_x = 0u;
    m.velocity_x = (int16_t)-0x0080;
    dk1_player_motion_integrate_horizontal(&m);
    assert(m.world_x == 99 && m.subpixel_x == 0x8000u);
    dk1_player_motion_integrate_horizontal(&m);
    assert(m.world_x == 99 && m.subpixel_x == 0u);

    m.velocity_y = 0;
    dk1_player_motion_apply_gravity(&m, false, 0);
    assert(m.velocity_y == (int16_t)-0x0070);
    m.velocity_y = (int16_t)-0x07F0;
    dk1_player_motion_apply_gravity(&m, false, 0);
    assert(m.velocity_y == (int16_t)-0x0800);
    m.velocity_y = 0;
    dk1_player_motion_apply_gravity(&m, true, (int16_t)-0x0090);
    assert(m.velocity_y == (int16_t)-0x0090);

    m.velocity_y = (int16_t)-0x013C;
    dk1_player_motion_apply_light_gravity(&m, false);
    assert(m.velocity_y == (int16_t)-0x0140);
    m.velocity_y = (int16_t)-0x01FC;
    dk1_player_motion_apply_light_gravity(&m, true);
    assert(m.velocity_y == (int16_t)-0x0200);

    assert(dk1_player_motion_response(0x0100u, 0u) == 0x0020u);
    assert(dk1_player_motion_response(0x0100u, 5u) == 0x0001u);
    assert(dk1_player_motion_response(0x0100u, 8u) == 0x0018u);

    memset(&m, 0, sizeof(m));
    m.target_velocity_x = 0x0180;
    dk1_player_motion_approach_horizontal(&m, 0u);
    assert(m.velocity_x == 0x0030);
    while (m.velocity_x != 0x0180) dk1_player_motion_approach_horizontal(&m, 0u);
    m.target_velocity_x = (int16_t)-0x0180;
    dk1_player_motion_approach_horizontal(&m, 7u);
    assert(m.velocity_x == 0);
    dk1_player_motion_approach_horizontal(&m, 7u);
    assert(m.velocity_x == (int16_t)-0x00C0);

    return 0;
}
