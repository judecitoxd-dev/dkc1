#include "dk1/barrel_runtime.h"
#include <string.h>

static bool type_uses_common_dispatch(uint16_t type_id) {
    return type_id == DK1_OBJECT_TYPE_STEEL_KEG ||
           type_id == DK1_OBJECT_TYPE_BARREL ||
           type_id == DK1_OBJECT_TYPE_ROPE_BARREL ||
           type_id == DK1_OBJECT_TYPE_DK_BARREL ||
           type_id == DK1_OBJECT_TYPE_TNT_BARREL;
}

static bool type_breaks_directly(uint16_t type_id) {
    return type_id == DK1_OBJECT_TYPE_ROPE_BARREL ||
           type_id == DK1_OBJECT_TYPE_DK_BARREL ||
           type_id == DK1_OBJECT_TYPE_TNT_BARREL;
}

static void change_state(Dk1BarrelObject *barrel, uint16_t state,
                         Dk1BarrelRuntimeResult *result) {
    if (barrel->state != state) result->state_changed = true;
    barrel->state = state;
}

static void add_conditional_renderer(const Dk1BarrelObject *barrel,
                                     Dk1BarrelRuntimeResult *result) {
    if ((int16_t)barrel->flags_1271 >= 0) {
        result->required_calls |= DK1_BARREL_CALL_BDF503;
        result->render_requested = true;
    }
}

static void animation_only(Dk1BarrelRuntimeResult *result) {
    result->required_calls |= DK1_BARREL_CALL_BE80E1;
    result->path = DK1_BARREL_PATH_ANIMATION_ONLY;
}

static void animation_render(const Dk1BarrelObject *barrel,
                             Dk1BarrelRuntimeResult *result) {
    result->required_calls |= DK1_BARREL_CALL_BE80E1;
    add_conditional_renderer(barrel, result);
    result->path = DK1_BARREL_PATH_ANIMATION_RENDER;
}

static void motion_render(const Dk1BarrelObject *barrel,
                          Dk1BarrelRuntimeResult *result) {
    result->required_calls |= DK1_BARREL_CALL_B174 |
                              DK1_BARREL_CALL_BFB1D5 |
                              DK1_BARREL_CALL_A8E3 |
                              DK1_BARREL_CALL_FBE7 |
                              DK1_BARREL_CALL_A5BE |
                              DK1_BARREL_CALL_BE80E1;
    add_conditional_renderer(barrel, result);
    result->path = DK1_BARREL_PATH_MOTION_RENDER;
}

static void post_motion_render(const Dk1BarrelObject *barrel,
                               Dk1BarrelRuntimeResult *result) {
    result->required_calls |= DK1_BARREL_CALL_A8E3 |
                              DK1_BARREL_CALL_FBE7 |
                              DK1_BARREL_CALL_A5BE |
                              DK1_BARREL_CALL_BE80E1;
    add_conditional_renderer(barrel, result);
    result->path = DK1_BARREL_PATH_POST_MOTION_RENDER;
}

static void explode_direct(Dk1BarrelRuntimeResult *result) {
    result->required_calls |= DK1_BARREL_CALL_BDF48B |
                              DK1_BARREL_CALL_BE96FE |
                              DK1_BARREL_CALL_D311 |
                              DK1_BARREL_CALL_B58000;
    result->spawn_script_count = 2u;
    result->destroyed = true;
    result->exploded = true;
    result->path = DK1_BARREL_PATH_EXPLODE;
}

static void destroy_common(const Dk1BarrelObject *barrel, bool with_setup,
                           Dk1BarrelRuntimeResult *result) {
    if (with_setup) result->required_calls |= DK1_BARREL_CALL_D32C;
    if (barrel->type_id == DK1_OBJECT_TYPE_TNT_BARREL) {
        explode_direct(result);
        return;
    }
    if (barrel->type_id == DK1_OBJECT_TYPE_DK_BARREL) {
        result->required_calls |= DK1_BARREL_CALL_BCBC47 |
                                  DK1_BARREL_CALL_CA43;
        result->released_kong = true;
    }
    result->required_calls |= DK1_BARREL_CALL_BDF48B |
                              DK1_BARREL_CALL_D2FC |
                              DK1_BARREL_CALL_FB75;
    result->sound_id = DK1_SOUND_BREAK_BARREL;
    result->destroyed = true;
    result->path = DK1_BARREL_PATH_DESTROY;
}

static void steel_cleanup(Dk1BarrelObject *barrel,
                          Dk1BarrelObject *linked,
                          Dk1BarrelRuntimeResult *result) {
    barrel->animation_speed = 0u;
    if (linked != NULL) linked->secondary_link_16f5 = 0u;
    barrel->velocity_y = 0x0500;
    barrel->velocity_x = 0;
    barrel->field_0f25 = 0u;
    change_state(barrel, 7u, result);
    result->path = DK1_BARREL_PATH_RETURN;
}

bool dk1_barrel_state_is_translated(uint16_t state) {
    return state < DK1_BARREL_STATE_COUNT;
}

bool dk1_barrel_runtime_step(Dk1BarrelObject *barrel,
                             Dk1BarrelRuntimeInputs in,
                             Dk1BarrelRuntimeResult *result) {
    if (barrel == NULL || result == NULL ||
        !type_uses_common_dispatch(barrel->type_id) ||
        !dk1_barrel_state_is_translated(barrel->state)) return false;
    memset(result, 0, sizeof(*result));
    result->translated = true;
    result->path = DK1_BARREL_PATH_RETURN;

    switch (barrel->state) {
    case 0u: /* $BF:CF2B */
        barrel->animation_speed = 0x0010u;
        barrel->state = 0u;
        barrel->field_0f25 = 0u;
        result->required_calls |= DK1_BARREL_CALL_CD8F;
        if (in.cd8f_carry) animation_render(barrel, result);
        else motion_render(barrel, result);
        break;

    case 1u: /* $BF:CF38 */
        result->required_calls |= DK1_BARREL_CALL_CD8F;
        if (in.cd8f_carry) animation_render(barrel, result);
        else motion_render(barrel, result);
        break;

    case 2u: /* $BF:CF3F */
        result->required_calls |= DK1_BARREL_CALL_CDEB;
        if (in.cdeb_carry) {
            animation_only(result);
            break;
        }
        result->required_calls |= DK1_BARREL_CALL_CD54 |
                                  DK1_BARREL_CALL_D329;
        if (!in.d329_carry) {
            animation_only(result);
            break;
        }
        if (barrel->type_id == DK1_OBJECT_TYPE_STEEL_KEG) {
            steel_cleanup(barrel, in.linked, result);
        } else {
            if (in.linked != NULL) in.linked->secondary_link_16f5 = 0u;
            destroy_common(barrel, false, result);
        }
        break;

    case 3u: { /* $BF:CFCC-$BF:D104 */
        bool collision_carry;
        if ((int16_t)barrel->timer_14c5 >= 0) {
            barrel->timer_14c5 = (uint16_t)(barrel->timer_14c5 - 1u);
            if (barrel->timer_14c5 == 0u) barrel->animation_speed = 0x0030u;
        }
        result->required_calls |= DK1_BARREL_CALL_CD8F;
        if (in.cd8f_carry) {
            animation_render(barrel, result);
            break;
        }
        result->required_calls |= DK1_BARREL_CALL_D263;
        if (in.d263_carry && type_breaks_directly(barrel->type_id)) {
            destroy_common(barrel, true, result);
            break;
        }
        result->required_calls |= DK1_BARREL_CALL_D23C;
        if (in.d23c_carry && barrel->type_id != DK1_OBJECT_TYPE_STEEL_KEG) {
            destroy_common(barrel, true, result);
            break;
        }
        if ((int16_t)barrel->flags_1271 < 0) {
            result->required_calls |= DK1_BARREL_CALL_D386;
            collision_carry = in.d386_carry;
        } else if ((int16_t)barrel->timer_14c5 < 0) {
            result->required_calls |= DK1_BARREL_CALL_D329;
            collision_carry = in.d329_carry;
        } else {
            result->required_calls |= DK1_BARREL_CALL_D324;
            collision_carry = in.d324_carry;
        }
        if (collision_carry && type_breaks_directly(barrel->type_id)) {
            destroy_common(barrel, false, result);
            break;
        }
        if ((int16_t)barrel->flags_1271 < 0) {
            result->required_calls |= DK1_BARREL_CALL_BDF42A;
            result->path = DK1_BARREL_PATH_HANDOFF;
            break;
        }
        result->required_calls |= DK1_BARREL_CALL_BBA4C8 |
                                  DK1_BARREL_CALL_BBA574;
        if (in.bba574_carry && in.owner != NULL &&
            in.owner->velocity_y < 0 &&
            barrel->type_id == DK1_OBJECT_TYPE_STEEL_KEG &&
            in.owner->primary_link_0512 == 0u &&
            (int32_t)in.owner->world_y - barrel->world_y >= 4) {
            in.owner->flags_1595 = 0x0040u;
            in.owner->world_x = (uint16_t)(barrel->world_x +
                ((barrel->flags_0c69 & 0x4000u) != 0u ? 8 : -8));
            result->required_calls |= DK1_BARREL_CALL_D20E |
                                      DK1_BARREL_CALL_D108;
            change_state(barrel, 3u, result);
        }
        if (barrel->type_id != DK1_OBJECT_TYPE_DK_BARREL) {
            result->required_calls |= DK1_BARREL_CALL_BDF44A;
        } else {
            result->required_calls |= DK1_BARREL_CALL_BDF7F2;
            if (in.bdf7f2_carry) destroy_common(barrel, true, result);
        }
        break;
    }

    case 4u: /* $BF:D117 */
        result->required_calls |= DK1_BARREL_CALL_CD8F;
        if (in.cd8f_carry) {
            animation_render(barrel, result);
            break;
        }
        result->required_calls |= DK1_BARREL_CALL_BBA4C8 |
                                  DK1_BARREL_CALL_BBA574;
        if (in.bba574_carry && in.owner != NULL &&
            in.owner->primary_link_0512 == 0u && in.owner->velocity_y < 0) {
            result->required_calls |= DK1_BARREL_CALL_D20E;
        } else {
            change_state(barrel, 2u, result);
        }
        result->required_calls |= DK1_BARREL_CALL_D29A |
                                  DK1_BARREL_CALL_D23C |
                                  DK1_BARREL_CALL_D329;
        break;

    case 5u: { /* $BF:D156 */
        int16_t compare;
        result->required_calls |= DK1_BARREL_CALL_CD8F;
        if (in.cd8f_carry) {
            animation_render(barrel, result);
            break;
        }
        barrel->timer_1491 = (uint16_t)(barrel->timer_1491 - 1u);
        if ((int16_t)barrel->timer_1491 < 0) {
            explode_direct(result);
            break;
        }
        compare = (int16_t)(uint16_t)(barrel->timer_1491 - 0x0030u);
        if (barrel->timer_1491 == 0x0030u) {
            barrel->velocity_x = 0x0300;
        } else if (compare < 0) {
            barrel->velocity_x = (int16_t)(-(int32_t)barrel->velocity_x);
        }
        post_motion_render(barrel, result);
        break;
    }

    case 6u: /* $BF:D185 */
        result->required_calls |= DK1_BARREL_CALL_BBA4D5 |
                                  DK1_BARREL_CALL_BBA574;
        if (in.bba574_carry) destroy_common(barrel, true, result);
        else animation_render(barrel, result);
        break;

    case 7u: /* $BF:D198 */
        result->required_calls |= DK1_BARREL_CALL_CDEB;
        if (in.cdeb_carry) {
            animation_only(result);
            break;
        }
        result->required_calls |= DK1_BARREL_CALL_CD54 |
                                  DK1_BARREL_CALL_D324;
        if (!in.d324_carry) {
            animation_only(result);
            break;
        }
        if (barrel->type_id == DK1_OBJECT_TYPE_STEEL_KEG) {
            steel_cleanup(barrel, in.linked, result);
        } else {
            if (in.linked != NULL) in.linked->secondary_link_16f5 = 0u;
            destroy_common(barrel, false, result);
        }
        break;

    case 8u: /* $BF:D1C7 */
        result->required_calls |= DK1_BARREL_CALL_B174 |
                                  DK1_BARREL_CALL_BFB1D5 |
                                  DK1_BARREL_CALL_B0BE |
                                  DK1_BARREL_CALL_BE80E1 |
                                  DK1_BARREL_CALL_BDF436;
        result->render_requested = true;
        result->path = DK1_BARREL_PATH_MOTION_RENDER;
        break;

    case 9u: /* $BF:D1DD */
        result->required_calls |= DK1_BARREL_CALL_CDEB;
        if (in.cdeb_carry) {
            animation_only(result);
            break;
        }
        if (in.linked == NULL || in.linked->link_1375 != in.current_offset ||
            in.linked->type_id != DK1_OBJECT_TYPE_MANKY_KONG) {
            result->required_calls |= DK1_BARREL_CALL_BDF48B;
            result->path = DK1_BARREL_PATH_DESPAWN;
            result->destroyed = true;
            break;
        }
        result->required_calls |= DK1_BARREL_CALL_CD54 |
                                  DK1_BARREL_CALL_BBA4D5 |
                                  DK1_BARREL_CALL_BBA574;
        if (in.bba574_carry) result->required_calls |= DK1_BARREL_CALL_C79E;
        break;

    default:
        return false;
    }
    return true;
}
