#include "dk1/object_executor.h"

typedef struct ExecuteBridge {
    Dk1ObjectWorld *world;
    const Dk1HostCallbackRegistry *registry;
    uint64_t frame_number;
    Dk1ObjectExecutionResult *result;
} ExecuteBridge;

static void execute_visit(
    size_t slot,
    Dk1ObjectPass pass,
    uint32_t callback_pc,
    void *user_data
) {
    ExecuteBridge *bridge = (ExecuteBridge *)user_data;
    Dk1HostCallbackContext context;
    Dk1HostInvokeResult invoke_result;
    (void)pass;
    if (bridge == 0 || slot >= DK1_OBJECT_SLOT_COUNT) return;
    context.frame_number = bridge->frame_number;
    context.object_slot = slot;
    context.source_pc = callback_pc;
    context.runtime = bridge->world;
    context.user_data = 0;
    invoke_result = dk1_host_callback_invoke(bridge->registry, &context);
    if (invoke_result == DK1_HOST_INVOKE_EXECUTED) {
        ++bridge->result->executed_callbacks;
    } else if (invoke_result == DK1_HOST_INVOKE_MISSING) {
        ++bridge->result->missing_callbacks;
    } else {
        ++bridge->result->failed_callbacks;
    }
}

Dk1ObjectExecutionResult dk1_object_execute_frame(
    Dk1ObjectWorld *world,
    const Dk1HostCallbackRegistry *registry,
    uint64_t frame_number
) {
    Dk1ObjectExecutionResult result = {0};
    ExecuteBridge bridge;
    if (world == 0) return result;
    bridge.world = world;
    bridge.registry = registry;
    bridge.frame_number = frame_number;
    bridge.result = &result;
    result.schedule = dk1_object_run_frame(
        &world->scheduler,
        execute_visit,
        &bridge
    );
    return result;
}
