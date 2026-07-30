#ifndef DK1_BOOT_MEMORY_H
#define DK1_BOOT_MEMORY_H

#include <stdbool.h>
#include <stdint.h>

typedef struct Dk1BootMemoryPlan {
    uint32_t wram_bytes_cleared;
    uint32_t vram_bytes_cleared;
    uint8_t wram_dma_passes;
    uint8_t vram_dma_passes;
    bool fixed_source_dma;
    uint32_t continuation_pc;
} Dk1BootMemoryPlan;

/*
 * Semantic clean-room model of the reset-time memory clearing sequence.
 * Source references: $00:A8C2 (VRAM clear) and $00:A8E6 (WRAM clear).
 */
Dk1BootMemoryPlan dk1_boot_memory_plan(void);

#endif
