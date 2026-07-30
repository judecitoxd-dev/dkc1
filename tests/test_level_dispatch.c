#include <assert.h>
#include "dk1/level_dispatch.h"

int main(void) {
    Dk1LevelDispatchEntry entry = {0};
    assert(dk1_level_dispatch_count() == 230u);

    assert(dk1_level_dispatch_get(0u, &entry));
    assert(entry.pre_frame_callback_80 == 0x8C80u);
    assert(entry.frame_callback_80 == 0x9738u);
    assert(entry.entry_callback_80 == 0x88F6u);
    assert(entry.bank_b9_callback_a == 0x8590u);
    assert(entry.bank_b9_callback_b == 0x861Bu);
    assert(entry.bank_b9_callback_c == 0x86FCu);

    assert(dk1_level_dispatch_get(10u, &entry));
    assert(entry.pre_frame_callback_80 == 0x900Eu);
    assert(entry.frame_callback_80 == 0x987Au);
    assert(entry.entry_callback_80 == 0x878Fu);
    assert(entry.bank_b9_callback_a == 0x890Fu);

    assert(dk1_level_dispatch_get(229u, &entry));
    assert(entry.pre_frame_callback_80 == 0x95C6u);
    assert(entry.frame_callback_80 == 0x99E9u);
    assert(entry.bank_b9_callback_b == 0x8EAAu);

    assert(!dk1_level_dispatch_get(230u, &entry));
    assert(!dk1_level_dispatch_get(0u, NULL));
    return 0;
}
