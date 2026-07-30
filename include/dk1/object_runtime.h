#ifndef DK1_OBJECT_RUNTIME_H
#define DK1_OBJECT_RUNTIME_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DK1_OBJECT_SLOT_COUNT 58u
#define DK1_PRIMARY_OBJECT_FIRST_SLOT 1u
#define DK1_PRIMARY_OBJECT_SLOT_COUNT 25u
#define DK1_SECONDARY_OBJECT_FIRST_SLOT 26u
#define DK1_SECONDARY_OBJECT_SLOT_COUNT 32u
#define DK1_OBJECT_TYPE_COUNT 122u
#define DK1_OBJECT_CALLBACK_BANK 0xBFu

typedef enum Dk1ObjectPool {
    DK1_OBJECT_POOL_PRIMARY = 0,
    DK1_OBJECT_POOL_SECONDARY = 1,
} Dk1ObjectPool;

typedef struct Dk1ObjectCallback {
    uint16_t address;
    uint16_t bank_field;
} Dk1ObjectCallback;

typedef struct Dk1ObjectTypeDispatch {
    uint16_t callback_address_bf;
    uint16_t attributes;
} Dk1ObjectTypeDispatch;

/* Parallel WRAM arrays use an even byte offset (slot * 2). */
uint16_t dk1_object_slot_offset(size_t slot);
bool dk1_object_slot_in_pool(size_t slot, Dk1ObjectPool pool);
size_t dk1_object_pool_first_slot(Dk1ObjectPool pool);
size_t dk1_object_pool_slot_count(Dk1ObjectPool pool);

/*
 * $81:D931-$81:D93E and $BE:812E-$BE:813B form a 24-bit JML target from
 * callback address $1341,X and the low byte of callback bank field $130D,X.
 */
bool dk1_object_callback_is_set(Dk1ObjectCallback callback);
uint32_t dk1_object_callback_pc(Dk1ObjectCallback callback);
Dk1ObjectCallback dk1_object_callback_from_script(uint16_t address, uint8_t bank);

/* Address-only map for the four-byte records beginning at $BF:817C. */
size_t dk1_object_type_count(void);
bool dk1_object_type_dispatch_get(uint16_t type_id, Dk1ObjectTypeDispatch *entry);
uint8_t dk1_object_type_priority(Dk1ObjectTypeDispatch entry);
bool dk1_object_type_special_pass(Dk1ObjectTypeDispatch entry);

#endif
