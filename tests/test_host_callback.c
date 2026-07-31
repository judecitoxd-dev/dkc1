#include <assert.h>
#include "dk1/host_callback.h"

static bool callback(Dk1HostCallbackContext *context) {
    int *counter = (int *)context->user_data;
    ++*counter;
    return context->object_slot == 3u;
}

int main(void) {
    Dk1HostCallbackRegistry registry;
    Dk1HostCallbackContext context = {1u, 3u, 0xBF9001u, 0, 0};
    int counter = 0;
    dk1_host_callback_registry_init(&registry);
    assert(dk1_host_callback_register(&registry, 0xBF9001u, callback, &counter));
    assert(dk1_host_callback_invoke(&registry, &context) == DK1_HOST_INVOKE_EXECUTED);
    assert(counter == 1);
    context.source_pc = 0xBF9999u;
    assert(dk1_host_callback_invoke(&registry, &context) == DK1_HOST_INVOKE_MISSING);
    return 0;
}
