#include "dk1/camera.h"
#include <stddef.h>

void dk1_camera_clamp_original(
    Dk1CameraState *camera,
    Dk1CameraBounds bounds,
    uint16_t game_mode
) {
    if (camera == NULL) return;

    if (camera->x < bounds.min_x) {
        camera->x = bounds.min_x;
        camera->x_residual_0889 = 0u;
    } else if (camera->x > bounds.max_x) {
        camera->x = bounds.max_x;
        camera->x_residual_0889 = 0u;
    }

    if (camera->y < bounds.min_y) {
        camera->y = bounds.min_y;
        camera->y_residual_0897 = 0u;
    } else if (
        game_mode != DK1_CAMERA_UNBOUNDED_VERTICAL_MODE &&
        camera->y > bounds.max_y
    ) {
        camera->y = bounds.max_y;
        camera->y_residual_0897 = 0u;
    }
}

Dk1ViewportRect dk1_camera_viewport(
    Dk1CameraState camera,
    uint16_t width,
    uint16_t height
) {
    const Dk1ViewportRect rect = {camera.x, camera.y, width, height};
    return rect;
}
