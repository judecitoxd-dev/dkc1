#include <stdio.h>
#include "dk1/reset_state.h"

int main(void) {
    const Dk1BootState state = dk1_boot_state_after_entry();

    puts("dk1 clean-room PC workspace");
    printf("boot model: native=%s irq_disabled=%s fastrom=%s sp=$%04X\n",
           state.native_mode ? "yes" : "no",
           state.interrupts_disabled ? "yes" : "no",
           state.fast_rom_enabled ? "yes" : "no",
           (unsigned)state.stack_pointer);
    puts("gameplay status: not implemented");
    return 0;
}
