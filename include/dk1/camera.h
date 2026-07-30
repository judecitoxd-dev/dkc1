#ifndef DK1_CAMERA_H
#define DK1_CAMERA_H

#include <stdint.h>

#define DK1_SNES_VIEWPORT_WIDTH 256u
#define DK1_SNES_VIEWPORT_HEIGHT 224u
#define DK1_CAMERA_UNBOUNDED_VERTICAL_MODE 9u

typedef struct Dk1CameraState {
    int16_t x;
    int16_t y;
    uint16_t x_residual_0889;
    uint16_t y_residual_0897;
} Dk1CameraState;

typedef struct Dk1CameraBounds {
    int16_t min_x;
    int16_t max_x;
    int16_t min_y;
    int16_t max_y;
} Dk1CameraBounds;

typedef struct Dk1ViewportRect {
    int32_t left;
    int32_t top;
    uint16_t width;
    uint16_t height;
} Dk1ViewportRect;

/* Semantic model of $80:9C9D-$80:9CDD. */
void dk1_camera_clamp_original(
    Dk1CameraState *camera,
    Dk1CameraBounds bounds,
    uint16_t game_mode
);

Dk1ViewportRect dk1_camera_viewport(
    Dk1CameraState camera,
    uint16_t width,
    uint16_t height
);

#endif
