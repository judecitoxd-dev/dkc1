#include "dk1/boot_memory.h"

Dk1BootMemoryPlan dk1_boot_memory_plan(void) {
    const Dk1BootMemoryPlan plan = {
        .wram_bytes_cleared = 2u * 65536u,
        .vram_bytes_cleared = 65536u,
        .wram_dma_passes = 2,
        .vram_dma_passes = 1,
        .fixed_source_dma = true,
        .continuation_pc = 0x008020u,
    };
    return plan;
}
