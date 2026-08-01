#include <assert.h>
#include <string.h>
#include "dk1/barrel_runtime.h"

static Dk1BarrelObject barrel(uint16_t type, uint16_t state) {
    Dk1BarrelObject b;
    memset(&b, 0, sizeof(b));
    b.type_id = type;
    b.state = state;
    return b;
}

int main(void) {
    Dk1BarrelObject b, linked, owner;
    Dk1BarrelRuntimeInputs in;
    Dk1BarrelRuntimeResult r;

    assert(DK1_BARREL_DISPATCH_PC == 0xBFCF0Cu);
    assert(dk1_barrel_state_is_translated(0u));
    assert(dk1_barrel_state_is_translated(9u));
    assert(!dk1_barrel_state_is_translated(10u));

    memset(&in, 0, sizeof(in));
    b = barrel(DK1_OBJECT_TYPE_BARREL, 0u);
    b.animation_speed = 7u;
    b.field_0f25 = 9u;
    assert(dk1_barrel_runtime_step(&b, in, &r));
    assert(b.animation_speed == 0x10u && b.field_0f25 == 0u);
    assert(r.path == DK1_BARREL_PATH_MOTION_RENDER);
    assert((r.required_calls & (DK1_BARREL_CALL_CD8F |
                                DK1_BARREL_CALL_BFB1D5 |
                                DK1_BARREL_CALL_BE80E1)) ==
                               (DK1_BARREL_CALL_CD8F |
                                DK1_BARREL_CALL_BFB1D5 |
                                DK1_BARREL_CALL_BE80E1));

    memset(&linked, 0, sizeof(linked));
    linked.secondary_link_16f5 = 0x1234u;
    memset(&in, 0, sizeof(in));
    in.d329_carry = true;
    in.linked = &linked;
    b = barrel(DK1_OBJECT_TYPE_STEEL_KEG, 2u);
    b.animation_speed = 0x20u;
    b.velocity_x = 0x0180;
    assert(dk1_barrel_runtime_step(&b, in, &r));
    assert(b.state == 7u && b.velocity_y == 0x0500 && b.velocity_x == 0);
    assert(b.animation_speed == 0u && linked.secondary_link_16f5 == 0u);
    assert(r.state_changed && !r.destroyed);

    memset(&in, 0, sizeof(in));
    b = barrel(DK1_OBJECT_TYPE_BARREL, 2u);
    assert(dk1_barrel_runtime_step(&b, in, &r));
    assert(r.path == DK1_BARREL_PATH_ANIMATION_ONLY);

    memset(&in, 0, sizeof(in));
    in.d263_carry = true;
    b = barrel(DK1_OBJECT_TYPE_ROPE_BARREL, 3u);
    b.timer_14c5 = 1u;
    assert(dk1_barrel_runtime_step(&b, in, &r));
    assert(b.timer_14c5 == 0u && b.animation_speed == 0x30u);
    assert(r.destroyed && r.path == DK1_BARREL_PATH_DESTROY);
    assert(r.sound_id == DK1_SOUND_BREAK_BARREL);
    assert((r.required_calls & DK1_BARREL_CALL_D32C) != 0u);

    memset(&owner, 0, sizeof(owner));
    owner.velocity_y = -1;
    owner.world_y = 20;
    memset(&in, 0, sizeof(in));
    in.bba574_carry = true;
    in.owner = &owner;
    b = barrel(DK1_OBJECT_TYPE_STEEL_KEG, 3u);
    b.timer_14c5 = 0xFFFFu;
    b.world_x = 100u;
    b.world_y = 10;
    b.flags_0c69 = 0x4000u;
    assert(dk1_barrel_runtime_step(&b, in, &r));
    assert(owner.flags_1595 == 0x40u && owner.world_x == 108u);
    assert((r.required_calls & (DK1_BARREL_CALL_D20E | DK1_BARREL_CALL_D108)) ==
                               (DK1_BARREL_CALL_D20E | DK1_BARREL_CALL_D108));

    memset(&in, 0, sizeof(in));
    b = barrel(DK1_OBJECT_TYPE_TNT_BARREL, 5u);
    b.timer_1491 = 0x31u;
    assert(dk1_barrel_runtime_step(&b, in, &r));
    assert(b.timer_1491 == 0x30u && b.velocity_x == 0x0300);
    assert(r.path == DK1_BARREL_PATH_POST_MOTION_RENDER);

    b = barrel(DK1_OBJECT_TYPE_TNT_BARREL, 5u);
    b.timer_1491 = 0x21u;
    b.velocity_x = 0x0100;
    assert(dk1_barrel_runtime_step(&b, in, &r));
    assert(b.timer_1491 == 0x20u && b.velocity_x == -0x0100);

    b = barrel(DK1_OBJECT_TYPE_TNT_BARREL, 5u);
    b.timer_1491 = 0u;
    assert(dk1_barrel_runtime_step(&b, in, &r));
    assert(r.exploded && r.destroyed && r.spawn_script_count == 2u);
    assert((r.required_calls & (DK1_BARREL_CALL_BE96FE |
                                DK1_BARREL_CALL_D311 |
                                DK1_BARREL_CALL_B58000)) ==
                               (DK1_BARREL_CALL_BE96FE |
                                DK1_BARREL_CALL_D311 |
                                DK1_BARREL_CALL_B58000));

    memset(&in, 0, sizeof(in));
    in.bba574_carry = true;
    b = barrel(DK1_OBJECT_TYPE_DK_BARREL, 6u);
    assert(dk1_barrel_runtime_step(&b, in, &r));
    assert(r.destroyed && r.released_kong && r.sound_id == DK1_SOUND_BREAK_BARREL);
    assert((r.required_calls & (DK1_BARREL_CALL_BCBC47 | DK1_BARREL_CALL_CA43)) ==
                               (DK1_BARREL_CALL_BCBC47 | DK1_BARREL_CALL_CA43));

    memset(&in, 0, sizeof(in));
    b = barrel(DK1_OBJECT_TYPE_BARREL, 8u);
    assert(dk1_barrel_runtime_step(&b, in, &r));
    assert(r.render_requested);
    assert((r.required_calls & (DK1_BARREL_CALL_B0BE |
                                DK1_BARREL_CALL_BDF436)) ==
                               (DK1_BARREL_CALL_B0BE |
                                DK1_BARREL_CALL_BDF436));

    memset(&linked, 0, sizeof(linked));
    linked.type_id = DK1_OBJECT_TYPE_MANKY_KONG;
    linked.link_1375 = 6u;
    memset(&in, 0, sizeof(in));
    in.linked = &linked;
    in.current_offset = 6u;
    in.bba574_carry = true;
    b = barrel(DK1_OBJECT_TYPE_BARREL, 9u);
    assert(dk1_barrel_runtime_step(&b, in, &r));
    assert(!r.destroyed && (r.required_calls & DK1_BARREL_CALL_C79E) != 0u);

    linked.link_1375 = 8u;
    assert(dk1_barrel_runtime_step(&b, in, &r));
    assert(r.destroyed && r.path == DK1_BARREL_PATH_DESPAWN);

    b = barrel(DK1_OBJECT_TYPE_OIL_DRUM, 0u);
    assert(!dk1_barrel_runtime_step(&b, in, &r));
    return 0;
}
