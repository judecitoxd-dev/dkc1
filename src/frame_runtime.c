#include "dk1/frame_runtime.h"

Dk1FrameOutput dk1_frame_runtime_step(
    Dk1FrameRuntime *runtime,
    const Dk1HostCallbackRegistry *callbacks,
    uint16_t joy1,
    uint16_t joy2,
    uint16_t player_mode,
    uint16_t selection_index,
    uint16_t active_player_or_mode
) {
    Dk1FrameOutput output = {0};
    if (runtime == 0) return output;
    dk1_input_update_auto(&runtime->input, joy1, joy2);
    dk1_input_select_active(
        &runtime->input,
        player_mode,
        selection_index,
        active_player_or_mode
    );
    output.objects = dk1_object_execute_frame(
        &runtime->objects,
        callbacks,
        runtime->frame_number
    );
    output.scroll = dk1_ppu_scroll_half_parallax(
        (uint16_t)runtime->viewport.x,
        (uint16_t)runtime->viewport.y
    );
    dk1_render_queue_build(&output.render_queue, &runtime->objects, runtime->viewport);
    ++runtime->frame_number;
    return output;
}
