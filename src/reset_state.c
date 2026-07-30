#include "dk1/reset_state.h"

Dk1BootState dk1_boot_state_after_entry(void) {
    Dk1BootState state = {
        .native_mode = true,
        .interrupts_disabled = true,
        .decimal_mode = false,
        .fast_rom_enabled = true,
        .data_bank = 0,
        .direct_page = 0,
        .stack_pointer = 0x01FF,
    };
    return state;
}
