#include "dk1/player_motion.h"
#include <limits.h>

static int16_t add_clamped_floor(int16_t value, int16_t delta, int16_t floor_value) {
    int32_t next = (int32_t)value + delta;
    if (next < floor_value) next = floor_value;
    if (next > INT16_MAX) next = INT16_MAX;
    return (int16_t)next;
}

static int16_t signed_integer_byte(int16_t velocity) {
    uint16_t bits = (uint16_t)velocity;
    uint8_t high = (uint8_t)(bits >> 8u);
    return (int16_t)(int8_t)high;
}

static void integrate_axis(int16_t velocity, uint16_t *fraction, int16_t *position) {
    uint32_t sum;
    int32_t next;
    uint16_t fractional_velocity;
    if (fraction == 0 || position == 0) return;
    fractional_velocity = (uint16_t)(((uint16_t)velocity & 0x00FFu) << 8u);
    sum = (uint32_t)(*fraction) + fractional_velocity;
    *fraction = (uint16_t)sum;
    next = (int32_t)(*position) + signed_integer_byte(velocity) + (int32_t)(sum >> 16u);
    *position = (int16_t)(uint16_t)next;
}

void dk1_player_motion_apply_gravity(Dk1PlayerMotion *motion,
                                     bool owner_override,
                                     int16_t owner_gravity) {
    int16_t gravity;
    if (motion == 0) return;
    gravity = owner_override ? owner_gravity : (int16_t)-0x0070;
    motion->velocity_y = add_clamped_floor(motion->velocity_y, gravity,
                                           (int16_t)-0x0800);
}

void dk1_player_motion_apply_light_gravity(Dk1PlayerMotion *motion,
                                           bool stronger_terminal) {
    int16_t terminal;
    if (motion == 0) return;
    terminal = stronger_terminal ? (int16_t)-0x0200 : (int16_t)-0x0140;
    motion->velocity_y = add_clamped_floor(motion->velocity_y,
                                           (int16_t)-0x000C, terminal);
}

void dk1_player_motion_integrate_horizontal(Dk1PlayerMotion *motion) {
    if (motion == 0) return;
    integrate_axis(motion->velocity_x, &motion->subpixel_x, &motion->world_x);
}

void dk1_player_motion_integrate_vertical(Dk1PlayerMotion *motion) {
    if (motion == 0) return;
    integrate_axis(motion->velocity_y, &motion->subpixel_y, &motion->world_y);
}

void dk1_player_motion_integrate(Dk1PlayerMotion *motion) {
    dk1_player_motion_integrate_horizontal(motion);
    dk1_player_motion_integrate_vertical(motion);
}

uint16_t dk1_player_motion_response(uint16_t difference, uint8_t mode) {
    switch (mode) {
    case 0u: return (uint16_t)(difference >> 3u);
    case 1u: return (uint16_t)(difference >> 4u);
    case 2u: return (uint16_t)(difference >> 5u);
    case 3u: return (uint16_t)(difference >> 6u);
    case 4u: return (uint16_t)(difference >> 7u);
    case 5u: return (uint16_t)(difference >> 8u);
    case 6u: return (uint16_t)(difference >> 2u);
    case 7u: return (uint16_t)(difference >> 1u);
    case 8u:
        return (uint16_t)((difference >> 4u) + (difference >> 5u));
    default:
        return 0u;
    }
}

void dk1_player_motion_approach_horizontal(Dk1PlayerMotion *motion,
                                           uint8_t response_mode) {
    int32_t target;
    int32_t delta;
    uint16_t magnitude;
    uint16_t step;
    if (motion == 0) return;
    target = (int32_t)motion->base_velocity_x + motion->target_velocity_x;
    delta = target - motion->velocity_x;
    if (delta == 0) return;
    magnitude = (uint16_t)(delta < 0 ? -delta : delta);
    step = dk1_player_motion_response(magnitude, response_mode);
    if (step == 0u) {
        motion->velocity_x = (int16_t)target;
        return;
    }
    if (delta < 0) motion->velocity_x = (int16_t)(motion->velocity_x - (int16_t)step);
    else motion->velocity_x = (int16_t)(motion->velocity_x + (int16_t)step);
}
