#include <stdio.h>
#include "dk1/boot_memory.h"
#include "dk1/reset_state.h"

int main(void) {
    const Dk1BootState state = dk1_boot_state_after_entry();
    const Dk1BootMemoryPlan memory = dk1_boot_memory_plan();

    puts("dk1 clean-room PC workspace");
    printf("boot model: native=%s irq_disabled=%s fastrom=%s sp=$%04X\n",
           state.native_mode ? "yes" : "no",
           state.interrupts_disabled ? "yes" : "no",
           state.fast_rom_enabled ? "yes" : "no",
           (unsigned)state.stack_pointer);
    printf("early clear plan: WRAM=%u KiB VRAM=%u KiB next=$%06X\n",
           (unsigned)(memory.wram_bytes_cleared / 1024u),
           (unsigned)(memory.vram_bytes_cleared / 1024u),
           (unsigned)memory.continuation_pc);
    puts("gameplay status: not implemented");
    return 0;
}
