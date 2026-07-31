#include "dk1/player_extended_runtime.h"
#include <string.h>

static int16_t clamp_add(int16_t value, int16_t delta, int16_t minimum, int16_t maximum) {
    int32_t next = (int32_t)value + delta;
    if (next < minimum) next = minimum;
    if (next > maximum) next = maximum;
    return (int16_t)next;
}

bool dk1_player_state_extended_is_translated(uint16_t state) {
    return state >= 21u && state <= 34u;
}

bool dk1_player_state20_plan(Dk1PlayerExtendedResult *result) {
    if (result == NULL) return false;
    memset(result, 0, sizeof(*result));
    result->translated = false;
    result->input_mode = 0u;
    result->required_calls = DK1_PLAYER_EXT_CALL_GUARD |
                             DK1_PLAYER_EXT_CALL_INPUT |
                             DK1_PLAYER_EXT_CALL_STATE20_HELPER |
                             DK1_PLAYER_EXT_CALL_MOVE |
                             DK1_PLAYER_EXT_CALL_A110 |
                             DK1_PLAYER_EXT_CALL_A5BE |
                             DK1_PLAYER_EXT_CALL_EVENTS;
    return true;
}

bool dk1_player_state_step_extended(Dk1PlayerExtendedFrame *f,
                                    Dk1PlayerExtendedInputs in,
                                    Dk1PlayerExtendedResult *r) {
    if (f == NULL || r == NULL || !dk1_player_state_extended_is_translated(f->state)) return false;
    memset(r, 0, sizeof(*r));
    r->translated = true;

    switch (f->state) {
    case 21u: /* $BF:9169 */
        r->required_calls = DK1_PLAYER_EXT_CALL_GUARD;
        if (in.guard_aborted) { r->finished = true; break; }
        r->input_mode = 0x000Cu;
        f->field_123d = 0u;
        r->required_calls |= DK1_PLAYER_EXT_CALL_INPUT | DK1_PLAYER_EXT_CALL_MOVE |
                             DK1_PLAYER_EXT_CALL_STATE20_HELPER | DK1_PLAYER_EXT_CALL_EVENTS;
        break;
    case 22u: /* $BF:9184 -> $BE:80E1 */
        r->required_calls = DK1_PLAYER_EXT_CALL_ANIMATION;
        break;
    case 23u: /* $BF:9188 */
        r->required_calls = DK1_PLAYER_EXT_CALL_GUARD;
        if (in.guard_aborted) { r->finished = true; break; }
        r->input_mode = 0u;
        r->required_calls |= DK1_PLAYER_EXT_CALL_INPUT | DK1_PLAYER_EXT_CALL_MOVE |
                             DK1_PLAYER_EXT_CALL_A75B | DK1_PLAYER_EXT_CALL_A110 |
                             DK1_PLAYER_EXT_CALL_A5BE | DK1_PLAYER_EXT_CALL_A51E |
                             DK1_PLAYER_EXT_CALL_EVENTS;
        break;
    case 24u: /* $BF:91A8 */
        r->required_calls = DK1_PLAYER_EXT_CALL_GUARD;
        if (in.guard_aborted) { r->finished = true; break; }
        r->input_mode = 2u;
        r->required_calls |= DK1_PLAYER_EXT_CALL_INPUT | DK1_PLAYER_EXT_CALL_MOVE |
                             DK1_PLAYER_EXT_CALL_EVENTS;
        break;
    case 25u: /* $BF:91BB */
        r->required_calls = DK1_PLAYER_EXT_CALL_GUARD;
        if (in.guard_aborted) { r->finished = true; break; }
        r->input_mode = 7u;
        r->required_calls |= DK1_PLAYER_EXT_CALL_INPUT | DK1_PLAYER_EXT_CALL_MOVE |
                             DK1_PLAYER_EXT_CALL_EVENTS;
        break;
    case 26u: /* $BF:91CE */
        r->required_calls = DK1_PLAYER_EXT_CALL_GUARD;
        if (in.guard_aborted) { r->finished = true; break; }
        r->input_mode = 0x000Cu;
        r->required_calls |= DK1_PLAYER_EXT_CALL_INPUT | DK1_PLAYER_EXT_CALL_8953 |
                             DK1_PLAYER_EXT_CALL_MOVE | DK1_PLAYER_EXT_CALL_A75B;
        break;
    case 27u: /* $BF:91E5 */
        f->global_1929 = 1u;
        f->velocity_x = 0x0200;
        f->subvelocity_x = 0x0200;
        f->state = 0x001Du;
        r->required_calls = DK1_PLAYER_EXT_CALL_92F9 | DK1_PLAYER_EXT_CALL_92D9 |
                             DK1_PLAYER_EXT_CALL_A29B;
        r->finished = true;
        break;
    case 28u: /* $BF:922B */
        if ((f->flags_12a5 & 1u) == 0u) {
            r->required_calls = DK1_PLAYER_EXT_CALL_MOVE;
            break;
        }
        f->velocity_x = 0x0200;
        f->subvelocity_x = 0x0200;
        f->state = 0x001Du;
        r->required_calls = DK1_PLAYER_EXT_CALL_92F9 | DK1_PLAYER_EXT_CALL_92D9;
        r->finished = true;
        break;
    case 29u: /* $BF:9250 */
        f->velocity_x = 0x0200;
        f->subvelocity_x = 0x0200;
        f->velocity_y = 0x0020;
        r->required_calls = DK1_PLAYER_EXT_CALL_857B | DK1_PLAYER_EXT_CALL_ANIMATION;
        if ((int32_t)f->world_x - (int32_t)f->camera_x >= 0x00F0) {
            f->state = 0x001Eu;
            f->global_051a = 0x820Fu;
            r->finished = true;
        }
        break;
    case 30u: /* $BF:928B */
        f->velocity_y = 0;
        r->required_calls = DK1_PLAYER_EXT_CALL_857B | DK1_PLAYER_EXT_CALL_ANIMATION;
        if ((f->global_051a & 0x000Fu) == 0u)
            r->required_calls |= DK1_PLAYER_EXT_CALL_B884B6;
        break;
    case 31u: /* $BF:92AA */
        f->velocity_x = (f->facing_145d & 1u) != 0u ? (int16_t)-0x0200 : (int16_t)0x0200;
        f->subvelocity_x = f->velocity_x;
        f->state = 0x0020u;
        f->field_11a1 = 0u;
        r->required_calls = DK1_PLAYER_EXT_CALL_92D9 | DK1_PLAYER_EXT_CALL_92F9 |
                             DK1_PLAYER_EXT_CALL_A29B;
        r->finished = true;
        break;
    case 32u: /* $BF:932A */
        f->velocity_y = 0;
        f->field_11a1 = 0u;
        r->required_calls = DK1_PLAYER_EXT_CALL_857B | DK1_PLAYER_EXT_CALL_ANIMATION;
        break;
    case 33u: /* $BF:933D */
        r->required_calls = DK1_PLAYER_EXT_CALL_GUARD;
        if (in.guard_aborted) { r->finished = true; break; }
        f->subvelocity_x = 0;
        f->velocity_y = clamp_add(f->velocity_y, (int16_t)-0x0030, (int16_t)-0x0500, INT16_MAX);
        r->required_calls |= DK1_PLAYER_EXT_CALL_MOVE;
        break;
    case 34u: /* $BF:9365 */
        r->required_calls = DK1_PLAYER_EXT_CALL_GUARD;
        if (in.guard_aborted) { r->finished = true; break; }
        r->input_mode = 8u;
        r->required_calls |= DK1_PLAYER_EXT_CALL_INPUT | DK1_PLAYER_EXT_CALL_8953 |
                             DK1_PLAYER_EXT_CALL_MOVE | DK1_PLAYER_EXT_CALL_A75B |
                             DK1_PLAYER_EXT_CALL_A599 | DK1_PLAYER_EXT_CALL_EVENTS;
        break;
    default:
        return false;
    }
    return true;
}
