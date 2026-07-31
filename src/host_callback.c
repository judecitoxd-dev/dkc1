#include "dk1/host_callback.h"

void dk1_host_callback_registry_init(Dk1HostCallbackRegistry *registry) {
    if (registry != 0) registry->count = 0u;
}

bool dk1_host_callback_register(
    Dk1HostCallbackRegistry *registry,
    uint32_t source_pc,
    Dk1HostCallbackFn function,
    void *user_data
) {
    size_t i = 0u;
    if (registry == 0 || function == 0 || source_pc > 0xFFFFFFu) return false;
    for (i = 0u; i < registry->count; ++i) {
        if (registry->entries[i].source_pc == source_pc) {
            registry->entries[i].function = function;
            registry->entries[i].user_data = user_data;
            return true;
        }
    }
    if (registry->count >= DK1_HOST_CALLBACK_CAPACITY) return false;
    registry->entries[registry->count].source_pc = source_pc;
    registry->entries[registry->count].function = function;
    registry->entries[registry->count].user_data = user_data;
    ++registry->count;
    return true;
}

const Dk1HostCallbackEntry *dk1_host_callback_find(
    const Dk1HostCallbackRegistry *registry,
    uint32_t source_pc
) {
    size_t i = 0u;
    if (registry == 0) return 0;
    for (i = 0u; i < registry->count; ++i) {
        if (registry->entries[i].source_pc == source_pc) return &registry->entries[i];
    }
    return 0;
}

Dk1HostInvokeResult dk1_host_callback_invoke(
    const Dk1HostCallbackRegistry *registry,
    Dk1HostCallbackContext *context
) {
    const Dk1HostCallbackEntry *entry = 0;
    if (context == 0) return DK1_HOST_INVOKE_FAILED;
    entry = dk1_host_callback_find(registry, context->source_pc);
    if (entry == 0) return DK1_HOST_INVOKE_MISSING;
    context->user_data = entry->user_data;
    return entry->function(context) ? DK1_HOST_INVOKE_EXECUTED : DK1_HOST_INVOKE_FAILED;
}
