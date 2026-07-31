#ifndef DK1_PLAYER_COMMON_H
#define DK1_PLAYER_COMMON_H

#include <stdbool.h>
#include <stdint.h>
#include "dk1/player_dispatch.h"

typedef struct Dk1PlayerCommonState {
    uint8_t player_index;
    uint16_t state;
    int16_t world_x;
    int16_t world_y;
    int16_t velocity_x;
    int16_t velocity_y;
    int16_t subvelocity_x;
    uint16_t flags_0c69;
    uint16_t partner_type;
    uint16_t effect_scale_1139;
    int16_t camera_lead_1a69;
    uint32_t callback_pc;
    uint32_t state_handler_pc;
} Dk1PlayerCommonState;

bool dk1_player_common_refresh_dispatch(Dk1PlayerCommonState *player);
bool dk1_player_common_clamp_horizontal(Dk1PlayerCommonState *player,
                                        int16_t level_min_x,
                                        int16_t level_max_x);
int16_t dk1_player_common_camera_lead(int16_t velocity_x, uint16_t flags_0c69);
uint16_t dk1_player_common_effect_scale(uint16_t partner_type,
                                        int16_t velocity_x,
                                        int16_t velocity_y,
                                        uint16_t previous_scale);

#endif
