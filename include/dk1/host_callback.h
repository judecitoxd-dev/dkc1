#ifndef DK1_HOST_CALLBACK_H
#define DK1_HOST_CALLBACK_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DK1_HOST_CALLBACK_CAPACITY 512u

typedef struct Dk1HostCallbackContext {
    uint64_t frame_number;
    size_t object_slot;
    uint32_t source_pc;
    void *runtime;
    void *user_data;
} Dk1HostCallbackContext;

typedef bool (*Dk1HostCallbackFn)(Dk1HostCallbackContext *context);

typedef struct Dk1HostCallbackEntry {
    uint32_t source_pc;
    Dk1HostCallbackFn function;
    void *user_data;
} Dk1HostCallbackEntry;

typedef struct Dk1HostCallbackRegistry {
    Dk1HostCallbackEntry entries[DK1_HOST_CALLBACK_CAPACITY];
    size_t count;
} Dk1HostCallbackRegistry;

typedef enum Dk1HostInvokeResult {
    DK1_HOST_INVOKE_MISSING = 0,
    DK1_HOST_INVOKE_EXECUTED,
    DK1_HOST_INVOKE_FAILED,
} Dk1HostInvokeResult;

void dk1_host_callback_registry_init(Dk1HostCallbackRegistry *registry);
bool dk1_host_callback_register(
    Dk1HostCallbackRegistry *registry,
    uint32_t source_pc,
    Dk1HostCallbackFn function,
    void *user_data
);
const Dk1HostCallbackEntry *dk1_host_callback_find(
    const Dk1HostCallbackRegistry *registry,
    uint32_t source_pc
);
Dk1HostInvokeResult dk1_host_callback_invoke(
    const Dk1HostCallbackRegistry *registry,
    Dk1HostCallbackContext *context
);

#endif
