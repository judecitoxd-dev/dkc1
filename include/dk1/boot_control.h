#ifndef DK1_BOOT_CONTROL_H
#define DK1_BOOT_CONTROL_H

#include <stdint.h>

typedef struct Dk1BootControlState {
    uint16_t player_or_mode_index_0042;
    uint16_t selection_index_0044;
    uint16_t active_player_056f;
    uint16_t boot_counter_003c;
    uint16_t frame_counter_0028;
    uint16_t raw_0016;
    uint16_t raw_0018;
    uint16_t raw_001a;
    uint16_t init_request_id;
    uint16_t followup_request_id;
} Dk1BootControlState;

Dk1BootControlState dk1_boot_control_state(uint16_t raw_flag_0525);

#endif
