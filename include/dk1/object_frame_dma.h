#ifndef DK1_OBJECT_FRAME_DMA_H
#define DK1_OBJECT_FRAME_DMA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/nmi_vram_dma.h"
#include "dk1/object_frame_layout.h"

typedef struct Dk1ObjectFrameDmaStats {
    size_t records;
    size_t bytes;
    uint32_t graphics_source_pc;
} Dk1ObjectFrameDmaStats;

bool dk1_object_frame_dma_schedule(const Dk1ObjectFrameLayout *layout,
                                   uint16_t tile_base_number,
                                   Dk1NmiVramDmaQueue *queue,
                                   Dk1ObjectFrameDmaStats *stats);

#endif
