#include <assert.h>
#include "dk1/frame_runtime.h"

static bool move(Dk1HostCallbackContext *context) {
    Dk1ObjectWorld *world = (Dk1ObjectWorld *)context->runtime;
    world->world_x[context->object_slot] += 1;
    return true;
}

int main(void) {
    Dk1FrameRuntime runtime = {0};
    Dk1HostCallbackRegistry registry;
    Dk1FrameOutput output;
    Dk1ObjectTypeDispatch type = {0};
    uint32_t callback_pc;
    runtime.viewport.width = 320u;
    runtime.viewport.height = 224u;
    runtime.viewport.margin = 16;
    runtime.objects.scheduler.type_id[1] = 1u;
    runtime.objects.world_x[1] = 10;
    runtime.objects.world_y[1] = 20;
    dk1_host_callback_registry_init(&registry);
    assert(dk1_object_type_dispatch_get(1u, &type));
    callback_pc = ((uint32_t)DK1_OBJECT_CALLBACK_BANK << 16u) | type.callback_address_bf;
    assert(dk1_host_callback_register(&registry, callback_pc, move, 0));
    output = dk1_frame_runtime_step(
        &runtime, &registry, DK1_BUTTON_RIGHT, 0u, 0u, 0u, 0u
    );
    assert(runtime.frame_number == 1u);
    assert(runtime.objects.world_x[1] == 11);
    assert(output.objects.executed_callbacks == 1u);
    assert(output.render_queue.count == 1u);
    assert(output.scroll.bg2_x == 0u);
    assert(dk1_input_pressed(&runtime.input, DK1_BUTTON_RIGHT));
    return 0;
}
