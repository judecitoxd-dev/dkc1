#ifndef DK1_NMI_VRAM_DMA_H
#define DK1_NMI_VRAM_DMA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/vram_image.h"

#define DK1_NMI_VRAM_DMA_MAX_RECORDS 64u
#define DK1_NMI_VRAM_DMA_ACTIVE 0x8000u

typedef struct Dk1NmiVramDmaRecord {
    uint16_t length;
    uint16_t vram_word_address;
    uint16_t source_address;
    uint16_t source_bank_active;
} Dk1NmiVramDmaRecord;

typedef struct Dk1NmiVramDmaQueue {
    Dk1NmiVramDmaRecord records[DK1_NMI_VRAM_DMA_MAX_RECORDS];
} Dk1NmiVramDmaQueue;

typedef bool (*Dk1NmiDmaRead)(void *context, uint32_t source_pc,
                              uint8_t *output, size_t size);

typedef struct Dk1NmiVramDmaStats {
    size_t records;
    size_t bytes;
} Dk1NmiVramDmaStats;

void dk1_nmi_vram_dma_init(Dk1NmiVramDmaQueue *queue);
bool dk1_nmi_vram_dma_push(Dk1NmiVramDmaQueue *queue, uint16_t length,
                           uint16_t vram_word_address, uint32_t source_pc);
bool dk1_nmi_vram_dma_execute(Dk1NmiVramDmaQueue *queue,
                              Dk1NmiDmaRead read_source,
                              void *context,
                              Dk1VramImage *vram,
                              Dk1NmiVramDmaStats *stats);

#endif
