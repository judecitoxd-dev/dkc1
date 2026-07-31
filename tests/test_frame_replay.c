#include <assert.h>
#include <string.h>
#include "dk1/frame_replay.h"
int main(void) {
    Dk1SceneMemory scene;
    Dk1FrameReplay replay;
    Dk1SceneRuntime initial = {{0u, 0u, 64u, 64u}, 0u};
    Dk1SceneRuntime a, b;
    memset(&scene, 0, sizeof(scene));
    scene.camera.maximum_x = 100u;
    dk1_frame_replay_init(&replay, 3u, initial);
    assert(dk1_frame_replay_append(&replay, DK1_RUNTIME_RIGHT));
    assert(dk1_frame_replay_append(&replay, DK1_RUNTIME_DOWN));
    assert(dk1_frame_replay_run(&scene, &replay, &a));
    assert(dk1_frame_replay_run(&scene, &replay, &b));
    assert(a.view.camera_x == 4u && a.view.camera_y == 4u && a.frame == 2u);
    assert(dk1_frame_replay_signature(&replay, &a) == dk1_frame_replay_signature(&replay, &b));
    return 0;
}
