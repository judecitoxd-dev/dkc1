#include "dk1/player_late_runtime.h"
#include <string.h>

static void set_animation(Dk1PlayerLateResult *result, uint16_t animation_id) {
    result->animation_id = animation_id;
    result->required_calls |= DK1_PLAYER_LATE_CALL_SET_ANIM;
}

static void sync_facing(uint16_t source_flags, uint16_t *destination_flags) {
    *destination_flags = (uint16_t)((((source_flags ^ *destination_flags) & 0x4000u)) ^ *destination_flags);
}

static void state_50(Dk1PlayerLateFrame *f, Dk1PlayerLateResult *r) {
    f->current.flags_1271 |= 0x8000u;
    if (f->other.state != 0x0025u) {
        f->current.state = 2u;
        set_animation(r, 7u);
        r->required_calls |= DK1_PLAYER_LATE_CALL_BCB882;
        return;
    }
    sync_facing(f->other.flags_0c69, &f->current.flags_0c69);
    f->current.world_x = f->other.world_x;
    f->current.world_y = f->other.world_y;
    if (f->current_slot_offset == 2u) {
        uint16_t delta = (uint16_t)(f->other.frame_0d11 - 0x0DC0u);
        if (delta < 0x0018u) {
            f->current.frame_0d11 = (uint16_t)(0x0714u + delta);
            return;
        }
    } else {
        uint16_t delta = (uint16_t)(f->other.frame_0d11 - 0x0714u);
        if (delta < 0x0018u) {
            f->current.frame_0d11 = (uint16_t)(0x0DC0u + delta);
            return;
        }
    }
    if (f->owner.type_16ad == 0x005Cu || f->owner.type_16ad == 0x005Du) {
        r->required_calls |= DK1_PLAYER_LATE_CALL_A9FC | DK1_PLAYER_LATE_CALL_ANIMATION;
    }
}

static void state_51(Dk1PlayerLateFrame *f, Dk1PlayerLateResult *r) {
    f->current.flags_1271 |= 0x8000u;
    if (f->other.state == 0x0029u) {
        f->current.frame_0d11 = 0u;
        return;
    }
    f->current.world_x = f->other.world_x;
    f->current.world_y = f->other.world_y;
    f->current.state = 2u;
    set_animation(r, 0x0017u);
    r->required_calls |= DK1_PLAYER_LATE_CALL_BCB882;
    f->current.frame_0d11 = 0u;
    f->current.cached_frame_0ae5 = 0u;
}

static void state_53(Dk1PlayerLateFrame *f, Dk1PlayerLateResult *r) {
    f->current.field_1375--;
    if ((f->current.field_1375 & 0x8000u) == 0u) {
        r->required_calls |= DK1_PLAYER_LATE_CALL_B0BE | DK1_PLAYER_LATE_CALL_ANIMATION;
        return;
    }
    f->current.field_11a1 = 0x00C1u;
    f->current.state = 0x002Bu;
    {
        int16_t swap = f->current.field_0b8d;
        f->current.field_0b8d = f->other.field_0b8d;
        f->other.field_0b8d = swap;
    }
    f->global_1929 = 0u;
    f->owner.flags_1699 &= 0xFFF7u;
    f->current.velocity_x = 0;
    f->other.velocity_x = 0;
    r->animation_id = 0x0060u;
    r->other_animation_id = 0x0060u;
    r->required_calls |= DK1_PLAYER_LATE_CALL_9B49 | DK1_PLAYER_LATE_CALL_A067 |
                         DK1_PLAYER_LATE_CALL_9B22 | DK1_PLAYER_LATE_CALL_SET_ANIM |
                         DK1_PLAYER_LATE_CALL_BCB882;
}

static void state_55(Dk1PlayerLateFrame *f, Dk1PlayerLateInputs in,
                     Dk1PlayerLateResult *r) {
    f->global_flags_0579 |= 2u;
    r->required_calls |= DK1_PLAYER_LATE_CALL_BDF7F2;
    if (in.bdf7f2_carry) {
        f->current.state = 0x000Cu;
        f->current.frame_0d11 = 0u;
        f->current.field_11a1 = 0u;
        return;
    }
    f->current.velocity_x = 0x0400;
    f->current.target_velocity_x = 0x0400;
    r->required_calls |= DK1_PLAYER_LATE_CALL_B0BE | DK1_PLAYER_LATE_CALL_867B;
}

static void state_57(Dk1PlayerLateFrame *f, Dk1PlayerLateResult *r) {
    f->current.field_1375--;
    if ((f->current.field_1375 & 0x8000u) != 0u) {
        r->required_calls |= DK1_PLAYER_LATE_CALL_80812F;
        return;
    }
    if (f->current.field_1375 == 0x0040u) {
        r->required_calls |= DK1_PLAYER_LATE_CALL_B6A87A | DK1_PLAYER_LATE_CALL_EFFECT;
    }
    f->current.world_x = (int16_t)((uint16_t)f->current.world_x ^ 4u);
    f->current.world_y = (int16_t)0xFE00u;
}

static void state_58(Dk1PlayerLateFrame *f, Dk1PlayerLateResult *r) {
    f->global_1929 = 1u;
    f->current.velocity_x = 0x0100;
    f->current.target_velocity_x = 0x0100;
    f->current.state = 0x003Du;
    r->required_calls |= DK1_PLAYER_LATE_CALL_92F9;
    if (f->owner.link_0512 == 0u) {
        set_animation(r, 0x0060u);
        r->required_calls |= DK1_PLAYER_LATE_CALL_A9FC;
    } else {
        r->animation_id = 0x0026u;
        r->required_calls |= DK1_PLAYER_LATE_CALL_BE8092;
    }
}

static void state_59(Dk1PlayerLateFrame *f, Dk1PlayerLateResult *r) {
    f->current.velocity_x = 0x0200;
    f->current.target_velocity_x = 0x0200;
    f->current.velocity_y = 0x0020;
    r->required_calls |= DK1_PLAYER_LATE_CALL_B0BE | DK1_PLAYER_LATE_CALL_REVERSE_ANIM;
    if ((int32_t)f->current.world_x - f->camera_x >= 0x0100) {
        f->current.state = 0x003Eu;
        f->global_051a = 0x820Fu;
        r->required_calls |= DK1_PLAYER_LATE_CALL_928A;
    }
}

static void state_63(Dk1PlayerLateFrame *f, Dk1PlayerLateResult *r) {
    int32_t distance;
    r->required_calls |= DK1_PLAYER_LATE_CALL_B0BE | DK1_PLAYER_LATE_CALL_ANIMATION;
    distance = (int32_t)f->current.world_x - f->camera_x;
    if (distance >= 0 && distance < 0x0080) return;
    r->required_calls |= DK1_PLAYER_LATE_CALL_9926 | DK1_PLAYER_LATE_CALL_SET_ANIM;
    f->current.state = 0x0042u;
    r->animation_id = 0x0061u;
    if ((f->global_flags_0579 & 1u) != 0u) {
        f->other.state = 0x0043u;
        r->other_animation_id = 0x0061u;
    }
}

static void state_68_reset(Dk1PlayerLateFrame *f, Dk1PlayerLateResult *r) {
    f->global_flags_0c69 = (f->game_mode == 7u || f->game_mode == 12u) ? 0x2000u : 0x3000u;
    if (f->owner.link_0512 != 0u) {
        f->owner.flags_1699 |= f->global_flags_0c69;
        f->current.state = 0x0014u;
    } else {
        f->current.state = 0u;
    }
    f->global_1929 = 0u;
    f->current.flags_0c69 |= f->global_flags_0c69;
    f->current.field_1631 = 0u;
    f->current.velocity_y = (int16_t)0xFE0Cu;
    f->other.flags_0c69 |= f->global_flags_0c69;
    r->required_calls |= DK1_PLAYER_LATE_CALL_FBE7;
    if (f->other.state == 0x0032u) {
        f->other.field_1631 = 0u;
        f->other.velocity_y = (int16_t)0xFE0Cu;
        r->required_calls |= DK1_PLAYER_LATE_CALL_A067 | DK1_PLAYER_LATE_CALL_BCB874;
    }
}

static void state_68(Dk1PlayerLateFrame *f, Dk1PlayerLateResult *r) {
    uint16_t facing = (uint16_t)(((uint16_t)f->current.velocity_x >> 1u) & 0x4000u);
    f->current.flags_0c69 = (uint16_t)((f->current.flags_0c69 & 0xBFFFu) | facing);
    f->other.flags_0c69 = (uint16_t)((f->other.flags_0c69 & 0xBFFFu) | facing);
    r->required_calls |= DK1_PLAYER_LATE_CALL_B0BE | DK1_PLAYER_LATE_CALL_AACB |
                         DK1_PLAYER_LATE_CALL_ANIMATION;
    if ((uint16_t)f->current.world_x >= 0xFC00u) return;
    if ((f->current.velocity_x < 0 && f->current.world_x < (int16_t)f->current.field_1375) ||
        (f->current.velocity_x >= 0 && f->current.world_x >= (int16_t)f->current.field_1375))
        state_68_reset(f, r);
}

bool dk1_player_state_late_is_translated(uint16_t state) {
    return state >= 50u && state <= 70u;
}

bool dk1_player_state_step_late(Dk1PlayerLateFrame *f,
                                Dk1PlayerLateInputs in,
                                Dk1PlayerLateResult *r) {
    if (f == NULL || r == NULL || !dk1_player_state_late_is_translated(f->current.state)) return false;
    memset(r, 0, sizeof(*r));
    r->translated = true;
    switch (f->current.state) {
    case 50u: state_50(f, r); break;
    case 51u: state_51(f, r); break;
    case 52u:
        f->current.flags_1271 |= 0x8000u;
        f->global_1917 &= 0xFFFCu;
        r->required_calls = DK1_PLAYER_LATE_CALL_C55A | DK1_PLAYER_LATE_CALL_8CC1 |
                            DK1_PLAYER_LATE_CALL_ANIMATION;
        break;
    case 53u: state_53(f, r); break;
    case 54u:
        r->required_calls = DK1_PLAYER_LATE_CALL_B0BE | DK1_PLAYER_LATE_CALL_ANIMATION;
        break;
    case 55u: state_55(f, in, r); break;
    case 56u:
        f->global_flags_0579 |= 2u;
        r->required_calls = DK1_PLAYER_LATE_CALL_8663;
        break;
    case 57u: state_57(f, r); break;
    case 58u: state_58(f, r); break;
    case 59u: state_59(f, r); break;
    case 60u:
        f->current.velocity_y = 0;
        r->required_calls = DK1_PLAYER_LATE_CALL_B0BE | DK1_PLAYER_LATE_CALL_929C;
        break;
    case 61u:
        f->current.velocity_y = 0;
        f->current.world_x = (int16_t)((uint16_t)f->current.world_x ^ 4u);
        f->current.world_y = (int16_t)0xFE00u;
        r->required_calls = DK1_PLAYER_LATE_CALL_929C;
        break;
    case 62u:
        r->required_calls = DK1_PLAYER_LATE_CALL_GUARD;
        if (in.guard_aborted) { r->finished = true; break; }
        r->input_mode = 1u;
        f->current.flags_0c69 &= 0xBFFFu;
        f->current.field_123d = 0u;
        r->required_calls |= DK1_PLAYER_LATE_CALL_INPUT | DK1_PLAYER_LATE_CALL_BF8589 |
                             DK1_PLAYER_LATE_CALL_A75B | DK1_PLAYER_LATE_CALL_8827;
        break;
    case 63u: state_63(f, r); break;
    case 64u:
        r->required_calls = DK1_PLAYER_LATE_CALL_ANIMATION;
        break;
    case 65u:
        f->current.flags_1271 |= 0x8000u;
        r->required_calls = DK1_PLAYER_LATE_CALL_ANIMATION;
        break;
    case 66u:
        r->required_calls = DK1_PLAYER_LATE_CALL_REVERSE_ANIM | DK1_PLAYER_LATE_CALL_929C;
        break;
    case 67u:
        f->current.field_1375--;
        if ((f->current.field_1375 & 0x8000u) != 0u)
            r->required_calls = DK1_PLAYER_LATE_CALL_80812F;
        else {
            if (f->current.field_1375 == 0x0040u)
                r->required_calls |= DK1_PLAYER_LATE_CALL_B6A87A | DK1_PLAYER_LATE_CALL_EFFECT;
            r->required_calls |= DK1_PLAYER_LATE_CALL_ANIMATION;
        }
        break;
    case 68u: state_68(f, r); break;
    case 69u:
        f->global_1a69 = 0x0030u;
        break;
    case 70u:
        r->required_calls = DK1_PLAYER_LATE_CALL_GUARD;
        if (in.guard_aborted) { r->finished = true; break; }
        f->global_1811 = (uint16_t)((f->global_1813 - 2u) & 0x003Fu);
        f->current.flags_12a5 = 0u;
        f->global_1a69 = 0u;
        f->current.field_145d--;
        r->required_calls |= DK1_PLAYER_LATE_CALL_B0BE | DK1_PLAYER_LATE_CALL_ANIMATION;
        if ((f->current.field_145d & 0x8000u) != 0u) f->current.state = 0x0028u;
        break;
    default: return false;
    }
    return true;
}
