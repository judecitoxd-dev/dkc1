#include "dk1/player_advanced_runtime.h"
#include <limits.h>
#include <string.h>

static int16_t arithmetic_shift_right5(int16_t value) {
    int32_t v = value;
    if (v >= 0) return (int16_t)(v / 32);
    return (int16_t)-(((-v) + 31) / 32);
}

static int16_t clamp_add(int16_t value, int16_t delta, int16_t minimum, int16_t maximum) {
    int32_t next = (int32_t)value + delta;
    if (next < minimum) next = minimum;
    if (next > maximum) next = maximum;
    return (int16_t)next;
}

static void set_animation(Dk1PlayerAdvancedResult *result, uint16_t animation_id) {
    result->animation_id = animation_id;
    result->required_calls |= DK1_PLAYER_ADV_CALL_SET_ANIM;
}

bool dk1_player_state_advanced_is_translated(uint16_t state) {
    return state >= 35u && state <= 49u;
}

bool dk1_player_state_step_advanced(Dk1PlayerAdvancedFrame *f,
                                    Dk1PlayerAdvancedInputs in,
                                    Dk1PlayerAdvancedResult *r) {
    int16_t saved;
    if (f == NULL || r == NULL || !dk1_player_state_advanced_is_translated(f->state)) return false;
    memset(r, 0, sizeof(*r));
    r->translated = true;

    switch (f->state) {
    case 35u: /* $BF:93FD */
        f->flags_1271 |= 0x8000u;
        f->timer_1375 = (uint16_t)(f->timer_1375 - 1u);
        if ((f->timer_1375 & 0x8000u) != 0u) {
            r->required_calls = DK1_PLAYER_ADV_CALL_9382;
            r->finished = true;
            break;
        }
        f->velocity_x = (int16_t)(f->velocity_x + f->accel_x);
        f->world_x = (int16_t)(f->source_x + arithmetic_shift_right5(f->velocity_x));
        f->velocity_y = (int16_t)(f->velocity_y + f->accel_y);
        f->world_y = (int16_t)(f->source_y + arithmetic_shift_right5(f->velocity_y));
        f->field_145d = clamp_add(f->field_145d, (int16_t)-0x0060, (int16_t)-0x0600, INT16_MAX);
        f->field_13e9 = (int16_t)(f->field_13e9 + f->field_145d);
        if (f->field_13e9 < 0) {
            f->field_0e21 = (int16_t)(f->field_0e21 + f->dp20);
            f->world_y = (int16_t)(f->world_y + f->dp22);
        } else {
            f->field_0e21 = (int16_t)(f->field_0e21 + f->dp24);
            f->world_y = (int16_t)(f->world_y + f->dp26);
        }
        r->required_calls = DK1_PLAYER_ADV_CALL_ANIMATION;
        break;
    case 36u: /* $BF:94A6 */
        r->required_calls = DK1_PLAYER_ADV_CALL_857B;
        f->frame_0d11 = 0u;
        f->field_0ae5 = 0u;
        if (f->partner.present) {
            f->partner.frame_0d11 = 0u;
            f->partner.field_0ae5 = 0u;
            f->partner.animation_speed = 0;
        }
        break;
    case 37u: /* $BF:94C7 */
        r->required_calls = DK1_PLAYER_ADV_CALL_GUARD;
        if (in.guard_aborted) { r->finished = true; break; }
        f->flags_0579 |= 0x0088u;
        if (!f->partner.present || f->partner.type != 0x0030u) {
            f->state = 0u;
            set_animation(r, 8u);
            r->finished = true;
            break;
        }
        f->world_x = f->partner.world_x;
        r->input_mode = 9u;
        r->input_argument = 0u;
        f->field_13e9 = f->accel_x;
        f->accel_x = 0;
        saved = f->velocity_y;
        f->velocity_y = (int16_t)(f->velocity_y + f->partner.field_14c5);
        r->required_calls |= DK1_PLAYER_ADV_CALL_A0F9 | DK1_PLAYER_ADV_CALL_INPUT |
                             DK1_PLAYER_ADV_CALL_B22E | DK1_PLAYER_ADV_CALL_A0D0 |
                             DK1_PLAYER_ADV_CALL_BF858C;
        f->velocity_y = saved;
        f->global_1a69 = 0u;
        if (f->partner.field_152d == 2u && f->partner.velocity_x >= 0)
            f->global_1a69 = 0x0050u;
        break;
    case 38u: /* $BF:954C */
        r->required_calls = DK1_PLAYER_ADV_CALL_GUARD;
        if (in.guard_aborted) { r->finished = true; break; }
        f->flags_0579 |= 0x0080u;
        if (!f->partner.present || f->partner.type != 0x0031u) {
            f->state = 0u;
            r->finished = true;
            break;
        }
        f->animation_speed = 0;
        f->flags_0c69 &= 0xBFFFu;
        r->input_mode = 0x000Au;
        saved = f->world_x;
        r->required_calls |= DK1_PLAYER_ADV_CALL_INPUT | DK1_PLAYER_ADV_CALL_BF858C |
                             DK1_PLAYER_ADV_CALL_EVENTS;
        f->global_1a69 = (uint16_t)((int16_t)((saved - f->world_x) * 2));
        f->global_1a69 = 0u;
        break;
    case 39u: /* $BF:961A */
        r->required_calls = DK1_PLAYER_ADV_CALL_GUARD;
        if (in.guard_aborted) { r->finished = true; break; }
        r->input_mode = 0x000Bu;
        r->required_calls |= DK1_PLAYER_ADV_CALL_INPUT | DK1_PLAYER_ADV_CALL_BF8589 |
                             DK1_PLAYER_ADV_CALL_A55F;
        if (!in.a55f_carry && f->owner_field_16ad != 0x0059u && f->state == 0x0027u)
            set_animation(r, 0x0059u);
        break;
    case 40u: /* $BF:964C */
        if (f->game_mode == 3u) {
            r->required_calls = DK1_PLAYER_ADV_CALL_A3CA;
            if (in.a3ca_aborted) { r->finished = true; break; }
        } else {
            r->required_calls = DK1_PLAYER_ADV_CALL_GUARD;
            if (in.guard_aborted) { r->finished = true; break; }
        }
        f->global_1811 = (uint16_t)((f->global_1813 - 2u) & 0x003Fu);
        f->flags_0579 |= 0x0088u;
        if (f->field_13e9 >= 0) --f->field_13e9;
        else { r->input_mode = 0x000Eu; r->required_calls |= DK1_PLAYER_ADV_CALL_INPUT; }
        if ((f->timer_1375 & 0x8000u) == 0u) {
            --f->timer_1375;
            if ((f->frame_counter & 1u) != 0u)
                r->required_calls |= DK1_PLAYER_ADV_CALL_EFFECT;
        } else if (f->game_mode == 3u) {
            f->field_11d5 = 2u;
            r->required_calls |= DK1_PLAYER_ADV_CALL_AFB2 | DK1_PLAYER_ADV_CALL_BFB1D5;
        } else {
            r->input_argument = 2u;
            r->required_calls |= DK1_PLAYER_ADV_CALL_AF81 | DK1_PLAYER_ADV_CALL_B268;
        }
        r->required_calls |= f->game_mode == 3u ? DK1_PLAYER_ADV_CALL_BF8672 : DK1_PLAYER_ADV_CALL_BF8590;
        f->global_1a69 = 0u;
        if (!f->partner.present) {
            r->required_calls |= DK1_PLAYER_ADV_CALL_A75B | DK1_PLAYER_ADV_CALL_EVENTS;
        } else {
            r->required_calls |= DK1_PLAYER_ADV_CALL_90CB | DK1_PLAYER_ADV_CALL_EVENTS;
            if (f->game_mode != 3u && (f->flags_12a5 & 0x0101u) == 0x0101u)
                f->state = 0x0014u;
        }
        break;
    case 41u: /* $BF:9702 */
        f->frame_0d11 = 0u;
        f->field_0ae5 = 0u;
        f->field_11a1 = 0u;
        f->owner_field_16e5 = 0u;
        if (f->partner.present) {
            f->partner.frame_0d11 = 0u;
            f->partner.field_0ae5 = 0u;
            f->partner.animation_speed = 0;
        }
        f->flags_12a5 = 0u;
        f->field_1631 = 1u;
        f->owner_flags_1699 = (uint16_t)((f->owner_flags_1699 | 0x0200u) & 0xFF7Fu);
        f->flags_0579 |= 0x0008u;
        r->input_mode = 0x000Du;
        r->input_argument = 0x000Cu;
        r->required_calls = DK1_PLAYER_ADV_CALL_INPUT | DK1_PLAYER_ADV_CALL_BBA4C8 |
                             DK1_PLAYER_ADV_CALL_871C;
        break;
    case 42u: /* $BF:9758 */
        r->required_calls = DK1_PLAYER_ADV_CALL_GUARD;
        if (in.guard_aborted) { r->finished = true; break; }
        f->flags_0579 |= 0x0008u;
        if ((f->frame_counter & 1u) != 0u) r->required_calls |= DK1_PLAYER_ADV_CALL_EFFECT;
        r->required_calls |= DK1_PLAYER_ADV_CALL_BF8590;
        f->global_1a69 = 0u;
        if ((int32_t)f->global_y_4a - f->world_y - f->camera_y < 0) {
            f->state = 0x0049u;
            f->global_051a = 0x820Fu;
            r->finished = true;
        }
        break;
    case 43u: /* $BF:9792 */
        r->required_calls = DK1_PLAYER_ADV_CALL_A3CA;
        if (in.a3ca_aborted) { r->finished = true; break; }
        r->input_mode = 0x000Fu;
        r->required_calls |= DK1_PLAYER_ADV_CALL_INPUT | DK1_PLAYER_ADV_CALL_8663 |
                             DK1_PLAYER_ADV_CALL_EVENTS;
        break;
    case 44u: /* $BF:97A5 and $BF:97C8 */
        r->required_calls = DK1_PLAYER_ADV_CALL_GUARD;
        if (in.guard_aborted) { r->finished = true; break; }
        r->input_mode = 0x000Cu;
        f->owner_flags_1699 |= 0x0200u;
        f->field_123d = 0u;
        r->required_calls |= DK1_PLAYER_ADV_CALL_INPUT | DK1_PLAYER_ADV_CALL_BF858C;
        if (f->field_145d != 0) {
            uint16_t magnitude = (uint16_t)f->field_145d & 0x7FFFu;
            if (magnitude == 0u) f->field_145d = 0x000Cu;
            --f->field_145d;
            if (((uint16_t)f->field_145d & 0x7FFFu) >= 7u) {
                if (f->velocity_y < 0) f->velocity_y = 0;
                break;
            }
        }
        f->velocity_y = clamp_add(f->velocity_y, (int16_t)-0x0070, (int16_t)-0x0300, INT16_MAX);
        break;
    case 45u: /* $BF:9805 */
        r->required_calls = DK1_PLAYER_ADV_CALL_GUARD;
        if (in.guard_aborted) { r->finished = true; break; }
        r->input_mode = 0u;
        r->input_argument = 6u;
        r->required_calls |= DK1_PLAYER_ADV_CALL_INPUT | DK1_PLAYER_ADV_CALL_BFB1DA |
                             DK1_PLAYER_ADV_CALL_BF8590;
        f->velocity_y = clamp_add(f->velocity_y, (int16_t)-0x0070, (int16_t)-0x0300, INT16_MAX);
        break;
    case 46u: /* $BF:9821 */
        r->required_calls = DK1_PLAYER_ADV_CALL_GUARD;
        if (in.guard_aborted) { r->finished = true; break; }
        f->flags_0579 |= 0x0080u;
        f->owner_flags_1699 |= 0x0200u;
        f->velocity_y = (int16_t)0xF000;
        f->velocity_x = 0;
        r->input_mode = 0x0010u;
        r->input_argument = 0x000Cu;
        r->required_calls |= DK1_PLAYER_ADV_CALL_INPUT | DK1_PLAYER_ADV_CALL_BBA4C8 |
                             DK1_PLAYER_ADV_CALL_871C | DK1_PLAYER_ADV_CALL_A485 |
                             DK1_PLAYER_ADV_CALL_A6E0 | DK1_PLAYER_ADV_CALL_EVENTS;
        break;
    case 47u: /* $BF:9864 */
        r->required_calls = DK1_PLAYER_ADV_CALL_GUARD;
        if (in.guard_aborted) { r->finished = true; break; }
        r->input_mode = 1u;
        f->flags_0c69 &= 0xBFFFu;
        f->field_123d = 0u;
        r->required_calls |= DK1_PLAYER_ADV_CALL_INPUT | DK1_PLAYER_ADV_CALL_8953 |
                             DK1_PLAYER_ADV_CALL_BF8589 | DK1_PLAYER_ADV_CALL_A75B |
                             DK1_PLAYER_ADV_CALL_A07C | DK1_PLAYER_ADV_CALL_8827;
        break;
    case 48u: /* $BF:9891 */
        r->required_calls = DK1_PLAYER_ADV_CALL_A3CA;
        if (in.a3ca_aborted) { r->finished = true; break; }
        r->input_mode = 0x0011u;
        r->input_argument = 2u;
        f->field_11d5 = 2u;
        r->required_calls |= DK1_PLAYER_ADV_CALL_INPUT | DK1_PLAYER_ADV_CALL_BFB1D5 |
                             DK1_PLAYER_ADV_CALL_B22E | DK1_PLAYER_ADV_CALL_BF8672 |
                             DK1_PLAYER_ADV_CALL_BBA4C8 | DK1_PLAYER_ADV_CALL_BBA58D |
                             DK1_PLAYER_ADV_CALL_EVENTS;
        if (in.bba58d_carry && in.bba58d_index >= 6u &&
            (!f->partner.present || in.bba58d_index != f->partner.field_0512) &&
            f->owner_field_16ad != 0x003Du) {
            set_animation(r, 0x003Au);
            f->velocity_y = 0;
            f->state = 0x004Cu;
            if (f->accel_x != 0) {
                uint16_t direction = (uint16_t)((uint16_t)f->accel_x >> 1u) & 0x4000u;
                f->flags_0c69 = (uint16_t)((f->flags_0c69 & 0xBFFFu) | direction);
            }
        }
        break;
    case 49u: /* $BF:990A and $BF:9926 */
        r->required_calls = DK1_PLAYER_ADV_CALL_B0BE | DK1_PLAYER_ADV_CALL_ANIMATION;
        if ((int32_t)f->world_x - f->camera_x >= 0 &&
            (int32_t)f->world_x - f->camera_x < f->timer_1375)
            break;
        {
            uint16_t shared_flags = f->game_mode == 7u ? 0x2000u : 0x3000u;
            if (f->partner.present) f->partner.flags_0c69 |= shared_flags;
            f->global_1929 = 0u;
            f->state = 0u;
            f->flags_0c69 |= shared_flags;
            f->field_1631 = 0u;
            f->velocity_y = (int16_t)0xFE0C;
            r->required_calls |= DK1_PLAYER_ADV_CALL_FBE7;
            if (f->other_player.present) {
                f->other_player.flags_0c69 |= shared_flags;
                if (f->other_player.state == 0x0032u) {
                    f->other_player.field_1631 = 0u;
                    f->other_player.velocity_y = (int16_t)0xFE0C;
                    r->required_calls |= DK1_PLAYER_ADV_CALL_A067 |
                                         DK1_PLAYER_ADV_CALL_BCB882;
                }
            }
            r->finished = true;
        }
        break;
    default:
        return false;
    }
    return true;
}
