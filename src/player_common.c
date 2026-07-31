#include "dk1/player_common.h"
#include <limits.h>

static uint16_t abs16(int16_t value) {
    if (value == INT16_MIN) return 0x8000u;
    return (uint16_t)(value < 0 ? -value : value);
}

bool dk1_player_common_refresh_dispatch(Dk1PlayerCommonState *player) {
    if (player == NULL || player->player_index > 1u ||
        player->state >= DK1_PLAYER_STATE_COUNT) return false;
    player->callback_pc = dk1_player_object_callback_pc(player->player_index);
    player->state_handler_pc = dk1_player_state_handler_pc(player->state);
    player->camera_lead_1a69 = dk1_player_common_camera_lead(
        player->velocity_x, player->flags_0c69);
    player->effect_scale_1139 = dk1_player_common_effect_scale(
        player->partner_type, player->velocity_x, player->velocity_y,
        player->effect_scale_1139);
    return true;
}

/* Translation of the horizontal guard at $BF:86E9-$BF:8717. */
bool dk1_player_common_clamp_horizontal(Dk1PlayerCommonState *player,
                                        int16_t level_min_x,
                                        int16_t level_max_x) {
    bool stopped = false;
    int32_t probe = 0;
    if (player == NULL) return false;
    if (player->velocity_x < 0) {
        probe = (int32_t)player->world_x - 0x12;
        if (probe < level_min_x) stopped = true;
    } else if (player->velocity_x > 0) {
        probe = (int32_t)player->world_x - 0xEE;
        if (probe >= level_max_x) stopped = true;
    }
    if (stopped) {
        player->velocity_x = 0;
        player->subvelocity_x = 0;
    }
    return stopped;
}

/* Translation of $BF:8607-$BF:8645. */
int16_t dk1_player_common_camera_lead(int16_t velocity_x, uint16_t flags_0c69) {
    const uint16_t speed = (uint16_t)(abs16(velocity_x) >> 6u);
    const int32_t scaled = (int32_t)speed + (int32_t)(speed >> 1u);
    const int32_t result = (flags_0c69 & 0x4000u) != 0u ? -scaled : scaled + 0x38;
    return (int16_t)result;
}

/* Translation of the partner-dependent scale update at $BF:8689-$BF:86E8. */
uint16_t dk1_player_common_effect_scale(uint16_t partner_type,
                                        int16_t velocity_x,
                                        int16_t velocity_y,
                                        uint16_t previous_scale) {
    uint16_t value = previous_scale;
    if (partner_type == 0x0060u) {
        value = previous_scale > 0x0100u ? (uint16_t)(previous_scale - 0x0010u) : 0x0100u;
        if (value < 0x0100u) value = 0x0100u;
    } else if (partner_type == 0x0021u || partner_type == 0x0029u) {
        const uint16_t ax = abs16(velocity_x);
        const uint16_t ay = abs16(velocity_y);
        value = ax > ay ? ax : ay;
        if (value < 0x00C0u) value = 0x00C0u;
    } else if (partner_type == 0x0065u) {
        value = 0x0200u;
    }
    return value;
}
