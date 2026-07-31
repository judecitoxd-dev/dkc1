#include "dk1/player_core_runtime.h"
#include <string.h>

static uint16_t abs16(int16_t value) {
    int32_t v = value;
    return (uint16_t)(v < 0 ? -v : v);
}

static void set_animation(Dk1PlayerCoreResult *r, uint16_t animation_id) {
    r->animation_id = animation_id;
    r->required_calls |= DK1_PLAYER_CORE_CALL_ANIMATION;
}

static void swap_links(Dk1PlayerCoreFrame *f) {
    int16_t value = f->current.field_0b8d;
    f->current.field_0b8d = f->other.field_0b8d;
    f->other.field_0b8d = value;
}

static void clear_owner_busy(Dk1PlayerCoreFrame *f) {
    f->owner.flags_1699 &= 0xFFF7u;
}

static void tail_8abb(Dk1PlayerCoreFrame *f, Dk1PlayerCoreResult *r) {
    if (f->current.field_1631 != 0u || (f->global_0579 & 0x2000u) == 0u) return;
    set_animation(r, 1u);
    f->current.state = 0x000Cu;
}

static void consume_facing_request(Dk1PlayerCoreFrame *f, Dk1PlayerCoreResult *r) {
    if ((f->global_1917 & 0x0010u) == 0u) return;
    f->global_1917 ^= 0x0010u;
    if (f->game_mode == 3u || f->owner.type_16ad == 7u ||
        f->owner.type_16ad == 0x0056u || f->owner.type_16ad == 0x005Au) {
        f->current.flags_0c69 ^= 0x4000u;
    } else {
        r->event_id = 6u;
        r->required_calls |= DK1_PLAYER_CORE_CALL_A9FC;
    }
}

static void request_facing_if_needed(Dk1PlayerCoreFrame *f) {
    int16_t delta = (int16_t)(f->other.world_x - f->current.world_x);
    uint16_t direction = delta < 0 ? 0x4000u : 0u;
    if ((direction ^ f->current.flags_0c69) & 0x4000u)
        f->global_1917 |= 0x0010u;
}

static void state2(Dk1PlayerCoreFrame *f, Dk1PlayerCoreResult *r) {
    uint16_t dx = abs16((int16_t)(f->other.world_x - f->current.world_x));
    uint16_t dy = abs16((int16_t)(f->other.world_y - f->current.world_y));
    f->current.flags_1271 |= 0x8000u;

    switch (f->other.state) {
    case 0x0026u:
        if (dx < 0x0018u) { f->current.state = 0x0033u; r->finished = true; return; }
        r->required_calls |= DK1_PLAYER_CORE_CALL_C486;
        break;
    case 0x0025u:
        if (dx < 0x000Cu) { f->current.state = 0x0034u; r->finished = true; return; }
        r->required_calls |= DK1_PLAYER_CORE_CALL_C486;
        break;
    case 0x0029u:
        if (dx < 0x0020u && dy < 0x0010u) {
            f->current.world_x = f->other.world_x;
            f->current.world_y = f->other.world_y;
            f->current.state = 0x0035u;
            r->required_calls |= DK1_PLAYER_CORE_CALL_BCB882;
            r->finished = true;
            return;
        }
        f->global_1917 |= 0x0020u;
        break;
    case 0x002Fu:
    case 0x0040u:
        f->global_1917 |= 0x0020u;
        break;
    default:
        if ((f->current.flags_12a5 & 0x1010u) != 0u)
            f->global_1917 |= 0x0020u;
        else
            r->required_calls |= DK1_PLAYER_CORE_CALL_C486 | DK1_PLAYER_CORE_CALL_8CC1 |
                                 DK1_PLAYER_CORE_CALL_8A8C;
        break;
    }

    if ((f->owner.flags_16cd & 1u) != 0u) {
        f->owner.flags_16cd &= 0xFFFEu;
        set_animation(r, 7u);
    }
    f->current.animation_speed = 0x0100;
    r->required_calls |= DK1_PLAYER_CORE_CALL_ANIMATION | DK1_PLAYER_CORE_CALL_A8FE;
    f->global_1917 &= 0xFFFCu;
    tail_8abb(f, r);
}

static void pair_launch(Dk1PlayerCoreFrame *f, Dk1PlayerCoreInputs in,
                        Dk1PlayerCoreResult *r) {
    int16_t magnitude;
    r->required_calls |= DK1_PLAYER_CORE_CALL_PAIR_LAUNCH;
    magnitude = in.pair_launch_velocity_valid ? in.pair_launch_velocity : 0;
    if (f->other.world_x < f->current.world_x) {
        f->other.velocity_x = magnitude;
        f->current.velocity_x = (int16_t)-magnitude;
    } else {
        f->current.velocity_x = magnitude;
        f->other.velocity_x = (int16_t)-magnitude;
    }
    f->current.velocity_y = 0x00FF;
    f->other.velocity_y = 0x00FF;
    f->current.field_1375 = 0x0020u;
    f->other.field_1375 = 0x0020u;
}

static void clamp_to_owner_floor(Dk1PlayerCoreFrame *f) {
    if (f->current.velocity_y < 0 && f->owner.anchor_y_16bd >= f->current.world_y)
        f->current.world_y = f->owner.anchor_y_16bd;
}

static void state3(Dk1PlayerCoreFrame *f, Dk1PlayerCoreInputs in,
                   Dk1PlayerCoreResult *r) {
    if ((f->current.field_1375 & 0x8000u) != 0u) {
        r->required_calls |= DK1_PLAYER_CORE_CALL_C121;
        pair_launch(f, in, r);
        return;
    }
    r->required_calls |= DK1_PLAYER_CORE_CALL_AF81 | DK1_PLAYER_CORE_CALL_AFE4 |
                         DK1_PLAYER_CORE_CALL_B012;
    clamp_to_owner_floor(f);
    f->current.field_1375--;
    if (f->current.field_1375 != 0u) {
        r->required_calls |= DK1_PLAYER_CORE_CALL_ANIMATION;
        return;
    }

    f->global_16e5 = 0u;
    f->global_16e7 = 0u;
    f->current.world_x = f->owner.anchor_x_16b9;
    f->current.world_y = f->owner.anchor_y_16bd;
    f->current.velocity_x = 0;
    f->current.state = 0u;
    r->required_calls |= DK1_PLAYER_CORE_CALL_A067;
    swap_links(f);
    f->global_1929 = 0u;
    clear_owner_busy(f);
    f->current.velocity_x = f->current.velocity_y = 0;
    f->other.velocity_x = f->other.velocity_y = 0;
    set_animation(r, 1u);
    r->required_calls |= DK1_PLAYER_CORE_CALL_BCB882;
    r->finished = true;
}

static void state4(Dk1PlayerCoreFrame *f, Dk1PlayerCoreResult *r) {
    if ((f->current.field_1375 & 0x8000u) != 0u) return;
    r->required_calls |= DK1_PLAYER_CORE_CALL_AF81 | DK1_PLAYER_CORE_CALL_AFE4 |
                         DK1_PLAYER_CORE_CALL_B012;
    clamp_to_owner_floor(f);
    r->required_calls |= DK1_PLAYER_CORE_CALL_ANIMATION;
}

static void state5(Dk1PlayerCoreFrame *f, Dk1PlayerCoreInputs in,
                   Dk1PlayerCoreResult *r) {
    int16_t delta = (int16_t)(f->owner.anchor_x_16b9 - f->current.world_x);
    uint16_t distance = abs16(delta);
    f->owner.flags_1699 |= 0x8000u;
    r->required_calls |= DK1_PLAYER_CORE_CALL_AF81;
    f->current.velocity_x = (int16_t)((uint16_t)delta << 4u);
    if ((distance >> 3u) == 0u) request_facing_if_needed(f);
    consume_facing_request(f, r);
    r->required_calls |= DK1_PLAYER_CORE_CALL_FBE7 | DK1_PLAYER_CORE_CALL_ANIMATION;
    if (abs16((int16_t)(f->other.world_x - f->current.world_x)) < 0x001Cu)
        r->animation_id = 3u;
    if (in.force_state2_branch) {
        clear_owner_busy(f);
        f->current.state = 2u;
        tail_8abb(f, r);
        return;
    }
    if (f->global_1927 != f->global_1813) {
        f->current.state = 6u;
        r->required_calls |= DK1_PLAYER_CORE_CALL_BCB83A;
        r->finished = true;
        return;
    }
    tail_8abb(f, r);
}

static void finish_state10_transition(Dk1PlayerCoreFrame *f,
                                      Dk1PlayerCoreInputs in,
                                      Dk1PlayerCoreResult *r,
                                      bool alternate) {
    f->global_0579 &= 0xFFFEu;
    f->current.state = alternate ? 0x002Bu : 1u;
    swap_links(f);
    if (f->global_056f != 0u) f->global_056f ^= 3u;
    f->global_1929 = 0u;
    clear_owner_busy(f);
    f->current.velocity_x = f->owner.launch_x_16c1;
    f->current.velocity_y = alternate ? 0x0080 : 0x0A00;
    if (!alternate) f->global_1b01 = 0u;
    r->required_calls |= DK1_PLAYER_CORE_CALL_B881FE;
    if (in.external_result_is_one) {
        f->current.state = alternate ? 0x0056u : 0x0053u;
        f->current.field_1375 = f->current.frame_0d11;
        f->current.field_14c5 = 0u;
        f->global_1929 = 3u;
    }
}

static void state10(Dk1PlayerCoreFrame *f, Dk1PlayerCoreInputs in,
                    Dk1PlayerCoreResult *r) {
    uint16_t speed_mask;
    r->required_calls |= DK1_PLAYER_CORE_CALL_AA08 | DK1_PLAYER_CORE_CALL_AFE4 |
                         DK1_PLAYER_CORE_CALL_B012;
    if (f->game_mode == 3u) {
        if (f->owner.type_16ad != 0x000Fu ||
            (f->current.velocity_x | f->current.velocity_y) != 0) {
            r->required_calls |= DK1_PLAYER_CORE_CALL_ANIMATION | DK1_PLAYER_CORE_CALL_A485;
            return;
        }
        set_animation(r, 0x0060u);
        finish_state10_transition(f, in, r, true);
        return;
    }

    speed_mask = (uint16_t)((abs16(f->current.velocity_x) |
                             abs16(f->current.velocity_y)) & 0xFFE0u);
    if (speed_mask != 0u) {
        r->required_calls |= DK1_PLAYER_CORE_CALL_ANIMATION | DK1_PLAYER_CORE_CALL_A485;
        f->current.field_11a1 = 0u;
        return;
    }

    f->global_16e5 = 0u;
    f->global_16e7 = 0u;
    f->current.field_11a1 = 0x00C0u;
    f->current.world_x = f->owner.anchor_x_16b9;
    f->current.world_y = f->owner.anchor_y_16bd;
    f->current.field_0db9 = (int16_t)0x8000u;
    f->current.field_0e21 = (int16_t)0x8000u;
    set_animation(r, 5u);
    finish_state10_transition(f, in, r, false);
}

bool dk1_player_state_core_is_translated(uint16_t state) {
    return state == 2u || state == 3u || state == 4u || state == 5u || state == 10u;
}

bool dk1_player_state_step_core(Dk1PlayerCoreFrame *f,
                                Dk1PlayerCoreInputs in,
                                Dk1PlayerCoreResult *r) {
    if (f == NULL || r == NULL || !dk1_player_state_core_is_translated(f->current.state))
        return false;
    memset(r, 0, sizeof(*r));
    r->translated = true;
    switch (f->current.state) {
    case 2u: state2(f, r); break;
    case 3u: state3(f, in, r); break;
    case 4u: state4(f, r); break;
    case 5u: state5(f, in, r); break;
    case 10u: state10(f, in, r); break;
    default: return false;
    }
    return true;
}
