#include "dk1/player_final_runtime.h"
#include <string.h>

static int16_t clamp_add(int16_t value, int16_t delta, int16_t minimum) {
    int32_t next = (int32_t)value + delta;
    if (next < minimum) next = minimum;
    return (int16_t)next;
}

static void common_state_72(Dk1PlayerFinalFrame *f,
                            Dk1PlayerFinalInputs in,
                            Dk1PlayerFinalResult *r) {
    r->required_calls = DK1_PLAYER_FINAL_CALL_GUARD;
    if (in.guard_aborted) { r->finished = true; return; }
    f->global_1811 = (uint16_t)((f->global_1813 - 2u) & 0x003Fu);
    r->required_calls |= DK1_PLAYER_FINAL_CALL_B0BE | DK1_PLAYER_FINAL_CALL_ANIMATION;
    f->current.flags_12a5 = 0u;
    f->global_1a69 = 0u;
    f->current.field_145d--;
    if ((f->current.field_145d & 0x8000u) != 0u) f->current.state = 0x0028u;
}

static void state_77_collision(Dk1PlayerFinalFrame *f,
                               Dk1PlayerFinalInputs in,
                               Dk1PlayerFinalResult *r) {
    r->required_calls |= DK1_PLAYER_FINAL_CALL_BBA4C8 | DK1_PLAYER_FINAL_CALL_BBA58D;
    r->helper_argument = 0x0022u;
    if (!in.collision_query_carry || in.collision_candidate_index < 6u ||
        in.collision_candidate_index == in.collision_owner_link) return;
    f->current.velocity_y = 0;
    f->linked.present = true;
    f->linked.object_index = in.collision_candidate_index;
    f->linked.request_1595 = 1u;
    f->linked.velocity_x = (int16_t)((uint16_t)f->current.velocity_x << 1u);
    f->linked.target_velocity_x = f->linked.velocity_x;
    r->callback_address = 0x884Bu;
    r->effect_id = 0x005Fu;
    r->required_calls |= DK1_PLAYER_FINAL_CALL_B5802F | DK1_PLAYER_FINAL_CALL_EFFECT;
}

static void state_78(Dk1PlayerFinalFrame *f,
                     Dk1PlayerFinalInputs in,
                     Dk1PlayerFinalResult *r) {
    f->global_1929 = 0x0011u;
    if ((f->global_051a & 0x000Fu) != 0u) return;
    r->required_calls |= DK1_PLAYER_FINAL_CALL_809BAA;
    if ((f->global_1e15 & 0x0020u) == 0u) {
        if (f->level_id == 0x0034u) f->global_0565 = 0x0034;
        else {
            f->global_0565 = (int16_t)(in.level_lookup_value & 0x00FFu);
            r->required_calls |= DK1_PLAYER_FINAL_CALL_BCBABD;
        }
    } else {
        f->global_0565 = (int16_t)(-f->global_0565);
    }
    f->global_0561 = f->global_0516;
    f->global_055f = in.selected_link_value;
    f->global_0559 = f->global_057b;
    f->global_055b = f->global_0579;
    f->global_0579 &= 0xFFFEu;
    f->global_0529 = 0u;
    f->global_057b = 0u;
    f->global_052b = 0u;
    f->global_052d = 0u;
    f->global_1929 = 0u;
    r->required_calls |= DK1_PLAYER_FINAL_CALL_B884B6;
    r->finished = true;
}

static void blink_state(Dk1PlayerFinalFrame *f,
                        Dk1PlayerFinalInputs in,
                        uint16_t expected_state,
                        Dk1PlayerFinalResult *r) {
    r->input_mode = 0x0012u;
    r->required_calls |= DK1_PLAYER_FINAL_CALL_INPUT;
    if (in.post_input_state_valid) f->current.state = in.post_input_state;
    if (f->current.state != expected_state) return;
    f->global_1e37 |= 1u;
    f->dp28--;
    f->dp4c = f->current.field_14c5;
    f->current.field_14c5 = (uint16_t)((f->current.field_14c5 + 1u) & 0x0FFFu);
    if (((f->current.field_14c5 ^ f->dp4c) & 0x0010u) == 0u) return;
    if ((f->dp4c & 0x0010u) == 0u) {
        f->current.frame_0d11 = 0u;
        f->current.cached_frame_0ae5 = 0u;
        return;
    }
    f->current.frame_0d11 = f->current.field_1375;
    r->effect_id = 0x002Du;
    r->required_calls |= DK1_PLAYER_FINAL_CALL_BFFB8F;
}

bool dk1_player_state_final_is_translated(uint16_t state) {
    return state >= 71u && state <= 86u;
}

bool dk1_player_state_step_final(Dk1PlayerFinalFrame *f,
                                 Dk1PlayerFinalInputs in,
                                 Dk1PlayerFinalResult *r) {
    if (f == NULL || r == NULL || !dk1_player_state_final_is_translated(f->current.state))
        return false;
    memset(r, 0, sizeof(*r));
    r->translated = true;
    switch (f->current.state) {
    case 71u:
        f->global_1a69 = 0x0030u;
        break;
    case 72u:
        common_state_72(f, in, r);
        break;
    case 73u:
        f->current.velocity_y = 0;
        r->required_calls = DK1_PLAYER_FINAL_CALL_857B | DK1_PLAYER_FINAL_CALL_BE80D2;
        if ((f->global_051a & 0x000Fu) == 0u) {
            r->required_calls |= DK1_PLAYER_FINAL_CALL_B884B6;
            r->finished = true;
        }
        break;
    case 74u:
        r->required_calls = DK1_PLAYER_FINAL_CALL_A3CA;
        if (in.a3ca_aborted) r->finished = true;
        else r->required_calls |= DK1_PLAYER_FINAL_CALL_8663;
        break;
    case 75u:
        f->global_0579 |= 2u;
        f->current.velocity_y = clamp_add(f->current.velocity_y, (int16_t)-0x0070, (int16_t)0xFA00);
        r->required_calls = DK1_PLAYER_FINAL_CALL_8666;
        break;
    case 76u:
        r->required_calls = DK1_PLAYER_FINAL_CALL_A3CA;
        if (in.a3ca_aborted) { r->finished = true; break; }
        f->current.field_11d5 = 2u;
        r->helper_argument = 2u;
        r->required_calls |= DK1_PLAYER_FINAL_CALL_B1D5 | DK1_PLAYER_FINAL_CALL_B22E |
                             DK1_PLAYER_FINAL_CALL_8672;
        break;
    case 77u:
        r->required_calls = DK1_PLAYER_FINAL_CALL_A3CA;
        if (in.a3ca_aborted) { r->finished = true; break; }
        f->current.field_11d5 = 2u;
        r->helper_argument = 2u;
        r->required_calls |= DK1_PLAYER_FINAL_CALL_B1D5 | DK1_PLAYER_FINAL_CALL_B22E |
                             DK1_PLAYER_FINAL_CALL_8672;
        state_77_collision(f, in, r);
        break;
    case 78u:
        state_78(f, in, r);
        break;
    case 79u:
        f->global_1929 = 3u;
        if ((f->global_051a & 0x000Fu) != 0u) break;
        f->global_057b = f->global_0559;
        f->global_0529 = f->global_0559;
        r->required_calls = DK1_PLAYER_FINAL_CALL_B89A81;
        f->global_0579 = (uint16_t)((((f->global_055b ^ f->global_0579) & 1u)) ^ f->global_0579);
        r->required_calls |= DK1_PLAYER_FINAL_CALL_B884B6;
        r->finished = true;
        break;
    case 80u:
        f->current.velocity_y = 0;
        r->required_calls = DK1_PLAYER_FINAL_CALL_929C;
        break;
    case 81u:
        r->required_calls = DK1_PLAYER_FINAL_CALL_A485 | DK1_PLAYER_FINAL_CALL_8778;
        break;
    case 82u:
        r->required_calls = DK1_PLAYER_FINAL_CALL_GUARD;
        if (in.guard_aborted) { r->finished = true; break; }
        r->input_mode = 7u;
        r->required_calls |= DK1_PLAYER_FINAL_CALL_INPUT | DK1_PLAYER_FINAL_CALL_BF8589 |
                             DK1_PLAYER_FINAL_CALL_A5BE | DK1_PLAYER_FINAL_CALL_A51E;
        break;
    case 83u:
        blink_state(f, in, 0x0053u, r);
        break;
    case 84u:
        r->required_calls = DK1_PLAYER_FINAL_CALL_BDF7F2;
        if (in.bdf7f2_carry) {
            r->required_calls |= DK1_PLAYER_FINAL_CALL_8DE8;
            break;
        }
        f->linked.present = true;
        f->linked.flags_1699 |= 2u;
        f->linked.field_16f9 = 8u;
        r->required_calls |= DK1_PLAYER_FINAL_CALL_BF8578 | DK1_PLAYER_FINAL_CALL_AACB |
                             DK1_PLAYER_FINAL_CALL_ANIMATION;
        break;
    case 85u:
        r->required_calls = DK1_PLAYER_FINAL_CALL_ANIMATION;
        break;
    case 86u:
        blink_state(f, in, 0x0056u, r);
        break;
    default:
        return false;
    }
    return true;
}
