#include <assert.h>
#include "dk1/camera.h"

int main(void) {
    const Dk1CameraBounds bounds = {-128, 2048, 0, 0x0200};
    Dk1CameraState camera = {-200, -10, 7u, 9u};
    Dk1ViewportRect rect;

    dk1_camera_clamp_original(&camera, bounds, 0u);
    assert(camera.x == -128);
    assert(camera.y == 0);
    assert(camera.x_residual_0889 == 0u);
    assert(camera.y_residual_0897 == 0u);

    camera.x = 3000;
    camera.y = 700;
    camera.x_residual_0889 = 3u;
    camera.y_residual_0897 = 4u;
    dk1_camera_clamp_original(&camera, bounds, 0u);
    assert(camera.x == 2048);
    assert(camera.y == 0x0200);
    assert(camera.x_residual_0889 == 0u);
    assert(camera.y_residual_0897 == 0u);

    camera.y = 700;
    camera.y_residual_0897 = 5u;
    dk1_camera_clamp_original(&camera, bounds, DK1_CAMERA_UNBOUNDED_VERTICAL_MODE);
    assert(camera.y == 700);
    assert(camera.y_residual_0897 == 5u);

    rect = dk1_camera_viewport(camera, 426u, DK1_SNES_VIEWPORT_HEIGHT);
    assert(rect.left == camera.x);
    assert(rect.top == camera.y);
    assert(rect.width == 426u);
    assert(rect.height == 224u);
    return 0;
}
