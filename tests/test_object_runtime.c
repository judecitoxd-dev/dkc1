#include <assert.h>
#include <stdint.h>
#include "dk1/object_runtime.h"

int main(void) {
    Dk1ObjectTypeDispatch entry = {0};
    Dk1ObjectCallback callback = dk1_object_callback_from_script(0xD931u, 0x81u);

    assert(DK1_OBJECT_SLOT_COUNT == 58u);
    assert(dk1_object_slot_offset(0u) == 0u);
    assert(dk1_object_slot_offset(57u) == 0x72u);
    assert(dk1_object_slot_offset(58u) == UINT16_MAX);
    assert(dk1_object_slot_in_pool(1u, DK1_OBJECT_POOL_PRIMARY));
    assert(dk1_object_slot_in_pool(25u, DK1_OBJECT_POOL_PRIMARY));
    assert(!dk1_object_slot_in_pool(26u, DK1_OBJECT_POOL_PRIMARY));
    assert(dk1_object_slot_in_pool(26u, DK1_OBJECT_POOL_SECONDARY));
    assert(dk1_object_slot_in_pool(57u, DK1_OBJECT_POOL_SECONDARY));

    assert(dk1_object_callback_is_set(callback));
    assert(dk1_object_callback_pc(callback) == 0x81D931u);
    callback.bank_field = 0xAB81u;
    assert(dk1_object_callback_pc(callback) == 0x81D931u);

    assert(dk1_object_type_count() == 122u);
    assert(dk1_object_type_dispatch_get(0u, &entry));
    assert(entry.callback_address_bf == 0x84A3u);
    assert(entry.attributes == 0x0030u);
    assert(dk1_object_type_priority(entry) == 0x30u);
    assert(!dk1_object_type_special_pass(entry));

    assert(dk1_object_type_dispatch_get(1u, &entry));
    assert(entry.callback_address_bf == 0x84ABu);
    assert(dk1_object_type_special_pass(entry));
    assert(dk1_object_type_priority(entry) == 0x30u);

    assert(dk1_object_type_dispatch_get(121u, &entry));
    assert(entry.callback_address_bf == 0xC05Cu);
    assert(entry.attributes == 0xB685u);
    assert(!dk1_object_type_dispatch_get(122u, &entry));
    assert(!dk1_object_type_dispatch_get(0u, NULL));
    return 0;
}
