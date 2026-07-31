#include <assert.h>
#include "dk1/object_executor.h"

static bool move_right(Dk1HostCallbackContext *context) {
    Dk1ObjectWorld *world = (Dk1ObjectWorld *)context->runtime;
    world->world_x[context->object_slot] += 2;
    return true;
}

int main(void) {
    Dk1ObjectWorld world = {0};
    Dk1HostCallbackRegistry registry;
    Dk1ObjectExecutionResult result;
    Dk1ObjectTypeDispatch type = {0};
    uint32_t callback_pc;
    dk1_host_callback_registry_init(&registry);
    assert(dk1_object_type_dispatch_get(1u, &type));
    callback_pc = ((uint32_t)DK1_OBJECT_CALLBACK_BANK << 16u) | type.callback_address_bf;
    world.scheduler.type_id[1] = 1u;
    world.scheduler.type_id[26] = 2u;
    assert(dk1_host_callback_register(&registry, callback_pc, move_right, 0));
    result = dk1_object_execute_frame(&world, &registry, 4u);
    assert(result.executed_callbacks == 1u);
    assert(result.missing_callbacks == 1u);
    assert(world.world_x[1] == 2);
    return 0;
}
