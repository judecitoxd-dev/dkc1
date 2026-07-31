#include <assert.h>
#include <string.h>
#include "dk1/player_final_runtime.h"

static void reset(Dk1PlayerFinalFrame *f, Dk1PlayerFinalResult *r, uint16_t state) {
    memset(f, 0, sizeof(*f));
    memset(r, 0, sizeof(*r));
    f->current.state = state;
}

int main(void) {
    Dk1PlayerFinalFrame f;
    Dk1PlayerFinalInputs in = {0};
    Dk1PlayerFinalResult r;
    uint16_t s;
    for (s = 71u; s <= 86u; ++s) assert(dk1_player_state_final_is_translated(s));
    assert(!dk1_player_state_final_is_translated(70u));
    assert(!dk1_player_state_final_is_translated(87u));

    reset(&f, &r, 71u);
    assert(dk1_player_state_step_final(&f, in, &r));
    assert(f.global_1a69 == 0x30u);

    reset(&f, &r, 72u);
    f.global_1813 = 2u;
    f.current.field_145d = 0u;
    assert(dk1_player_state_step_final(&f, in, &r));
    assert(f.global_1811 == 0u && f.current.state == 0x28u && f.global_1a69 == 0u);

    reset(&f, &r, 73u);
    f.current.velocity_y = 123;
    assert(dk1_player_state_step_final(&f, in, &r));
    assert(f.current.velocity_y == 0 && r.finished);

    reset(&f, &r, 75u);
    f.current.velocity_y = (int16_t)0xFA20;
    assert(dk1_player_state_step_final(&f, in, &r));
    assert(f.current.velocity_y == (int16_t)0xFA00 && (f.global_0579 & 2u));

    reset(&f, &r, 77u);
    f.current.velocity_x = 0x0200;
    in.collision_query_carry = true;
    in.collision_candidate_index = 8u;
    in.collision_owner_link = 6u;
    assert(dk1_player_state_step_final(&f, in, &r));
    assert(f.current.field_11d5 == 2u && f.current.velocity_y == 0);
    assert(f.linked.request_1595 == 1u && f.linked.velocity_x == 0x0400);
    assert(r.effect_id == 0x5Fu && r.callback_address == 0x884Bu);

    reset(&f, &r, 78u);
    f.level_id = 0x34u;
    f.global_051a = 0x1200u;
    f.global_0516 = 0x5678u;
    f.global_0579 = 3u;
    f.global_057b = 7u;
    in.selected_link_value = 0x1234u;
    assert(dk1_player_state_step_final(&f, in, &r));
    assert(r.finished && f.global_0565 == 0x34 && f.global_055f == 0x1234u && f.global_0561 == 0x5678u);
    assert(f.global_0559 == 7u && f.global_055b == 3u && f.global_0579 == 2u);

    reset(&f, &r, 79u);
    f.global_0559 = 5u;
    f.global_055b = 1u;
    f.global_0579 = 0u;
    assert(dk1_player_state_step_final(&f, in, &r));
    assert(r.finished && f.global_057b == 5u && f.global_0529 == 5u && f.global_0579 == 1u);

    reset(&f, &r, 83u);
    f.current.field_14c5 = 0x000Fu;
    f.current.field_1375 = 0x0330u;
    assert(dk1_player_state_step_final(&f, in, &r));
    assert(f.current.frame_0d11 == 0u && f.current.cached_frame_0ae5 == 0u);
    f.current.state = 83u;
    f.current.field_14c5 = 0x001Fu;
    assert(dk1_player_state_step_final(&f, in, &r));
    assert(f.current.frame_0d11 == 0x0330u && r.effect_id == 0x2Du);

    reset(&f, &r, 84u);
    assert(dk1_player_state_step_final(&f, in, &r));
    assert(f.linked.present && (f.linked.flags_1699 & 2u) && f.linked.field_16f9 == 8u);

    reset(&f, &r, 86u);
    f.current.field_14c5 = 0x001Fu;
    f.current.field_1375 = 0x0440u;
    assert(dk1_player_state_step_final(&f, in, &r));
    assert(f.current.frame_0d11 == 0x0440u);
    return 0;
}
