#include <assert.h>
#include "dk1/reset_state.h"

int main(void) {
    const Dk1BootState state = dk1_boot_state_after_entry();
    assert(state.native_mode);
    assert(state.interrupts_disabled);
    assert(!state.decimal_mode);
    assert(state.fast_rom_enabled);
    assert(state.data_bank == 0);
    assert(state.direct_page == 0);
    assert(state.stack_pointer == 0x01FF);
    return 0;
}
