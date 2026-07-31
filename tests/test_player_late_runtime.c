#include <assert.h>
#include <string.h>
#include "dk1/player_late_runtime.h"

int main(void) {
    Dk1PlayerLateFrame frame;
    Dk1PlayerLateResult result;
    Dk1PlayerLateInputs inputs = {0};

    memset(&frame, 0, sizeof(frame));
    frame.current.state = 50u;
    frame.current_slot_offset = 2u;
    frame.other.state = 0x25u;
    frame.other.frame_0d11 = 0x0DC5u;
    frame.other.world_x = 100;
    frame.other.world_y = 200;
    frame.other.flags_0c69 = 0x4000u;
    assert(dk1_player_state_step_late(&frame, inputs, &result));
    assert(frame.current.frame_0d11 == 0x0719u);
    assert(frame.current.world_x == 100 && frame.current.world_y == 200);
    assert((frame.current.flags_0c69 & 0x4000u) != 0u);

    memset(&frame, 0, sizeof(frame));
    frame.current.state = 55u;
    inputs.bdf7f2_carry = false;
    assert(dk1_player_state_step_late(&frame, inputs, &result));
    assert(frame.current.velocity_x == 0x0400);
    assert(frame.current.target_velocity_x == 0x0400);
    assert((result.required_calls & DK1_PLAYER_LATE_CALL_867B) != 0u);

    memset(&frame, 0, sizeof(frame));
    frame.current.state = 57u;
    frame.current.field_1375 = 0x0041u;
    assert(dk1_player_state_step_late(&frame, inputs, &result));
    assert(frame.current.field_1375 == 0x0040u);
    assert(frame.current.world_x == 4);
    assert((uint16_t)frame.current.world_y == 0xFE00u);
    assert((result.required_calls & DK1_PLAYER_LATE_CALL_EFFECT) != 0u);

    memset(&frame, 0, sizeof(frame));
    frame.current.state = 63u;
    frame.current.world_x = 200;
    frame.camera_x = 0;
    frame.global_flags_0579 = 1u;
    assert(dk1_player_state_step_late(&frame, inputs, &result));
    assert(frame.current.state == 0x42u && frame.other.state == 0x43u);
    assert(result.animation_id == 0x61u && result.other_animation_id == 0x61u);

    memset(&frame, 0, sizeof(frame));
    frame.current.state = 68u;
    frame.current.velocity_x = 0x0100;
    frame.current.world_x = 120;
    frame.current.field_1375 = 100u;
    frame.other.state = 0x32u;
    frame.owner.link_0512 = 1u;
    frame.game_mode = 7u;
    assert(dk1_player_state_step_late(&frame, inputs, &result));
    assert(frame.current.state == 0x14u);
    assert(frame.global_flags_0c69 == 0x2000u);
    assert((uint16_t)frame.current.velocity_y == 0xFE0Cu);
    assert((uint16_t)frame.other.velocity_y == 0xFE0Cu);

    memset(&frame, 0, sizeof(frame));
    frame.current.state = 70u;
    frame.current.field_145d = 0u;
    frame.global_1813 = 1u;
    assert(dk1_player_state_step_late(&frame, inputs, &result));
    assert(frame.current.state == 0x28u);
    assert(frame.global_1811 == 0x3Fu);

    assert(dk1_player_state_late_is_translated(50u));
    assert(dk1_player_state_late_is_translated(70u));
    assert(!dk1_player_state_late_is_translated(71u));
    return 0;
}
