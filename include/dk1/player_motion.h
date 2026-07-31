#ifndef DK1_PLAYER_MOTION_H
#define DK1_PLAYER_MOTION_H

#include <stdbool.h>
#include <stdint.h>

/* Portable view of the fixed-point fields used by $BF:AF81/$AFE4/$B012. */
typedef struct Dk1PlayerMotion {
    uint16_t world_x;
    int16_t world_y;
    uint16_t subpixel_x;
    uint16_t subpixel_y;
    int16_t velocity_x;
    int16_t velocity_y;
    int16_t base_velocity_x;
    int16_t target_velocity_x;
} Dk1PlayerMotion;

void dk1_player_motion_apply_gravity(Dk1PlayerMotion *motion,
                                     bool owner_override,
                                     int16_t owner_gravity);
void dk1_player_motion_apply_light_gravity(Dk1PlayerMotion *motion,
                                           bool stronger_terminal);
void dk1_player_motion_integrate_horizontal(Dk1PlayerMotion *motion);
void dk1_player_motion_integrate_vertical(Dk1PlayerMotion *motion);
void dk1_player_motion_integrate(Dk1PlayerMotion *motion);
uint16_t dk1_player_motion_response(uint16_t difference, uint8_t mode);
void dk1_player_motion_approach_horizontal(Dk1PlayerMotion *motion,
                                           uint8_t response_mode);

#endif
