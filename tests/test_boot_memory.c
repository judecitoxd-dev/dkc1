#include <assert.h>
#include "dk1/boot_memory.h"
#include "dk1/snes_registers.h"

int main(void) {
    const Dk1BootMemoryPlan plan = dk1_boot_memory_plan();
    assert(plan.wram_bytes_cleared == 128u * 1024u);
    assert(plan.vram_bytes_cleared == 64u * 1024u);
    assert(plan.wram_dma_passes == 2);
    assert(plan.vram_dma_passes == 1);
    assert(plan.fixed_source_dma);
    assert(plan.continuation_pc == 0x008020u);

    assert(dk1_snes_dma_length(0) == 65536u);
    assert(dk1_snes_dma_length(0x1234u) == 0x1234u);
    return 0;
}
