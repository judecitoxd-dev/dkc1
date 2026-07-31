#include "dk1/nmi_vram_dma.h"
#include <stdlib.h>
#include <string.h>

void dk1_nmi_vram_dma_init(Dk1NmiVramDmaQueue *queue) {
    if (queue != NULL) memset(queue, 0, sizeof(*queue));
}

bool dk1_nmi_vram_dma_push(Dk1NmiVramDmaQueue *queue, uint16_t length,
                           uint16_t vram_word_address, uint32_t source_pc) {
    size_t i;
    uint8_t bank;
    if (queue == NULL || length == 0u) return false;
    bank = (uint8_t)(source_pc >> 16u);
    for (i = 0u; i < DK1_NMI_VRAM_DMA_MAX_RECORDS; ++i) {
        Dk1NmiVramDmaRecord *record = &queue->records[i];
        if ((record->source_bank_active & DK1_NMI_VRAM_DMA_ACTIVE) != 0u) continue;
        record->length = length;
        record->vram_word_address = vram_word_address;
        record->source_address = (uint16_t)source_pc;
        record->source_bank_active = (uint16_t)(DK1_NMI_VRAM_DMA_ACTIVE | bank);
        return true;
    }
    return false;
}

bool dk1_nmi_vram_dma_execute(Dk1NmiVramDmaQueue *queue,
                              Dk1NmiDmaRead read_source,
                              void *context,
                              Dk1VramImage *vram,
                              Dk1NmiVramDmaStats *stats) {
    size_t i;
    Dk1NmiVramDmaStats local = {0u, 0u};
    if (queue == NULL || read_source == NULL || vram == NULL) return false;
    for (i = 0u; i < DK1_NMI_VRAM_DMA_MAX_RECORDS; ++i) {
        Dk1NmiVramDmaRecord *record = &queue->records[i];
        uint8_t *buffer;
        size_t j, destination;
        uint32_t source_pc;
        if ((record->source_bank_active & DK1_NMI_VRAM_DMA_ACTIVE) == 0u) break;
        buffer = (uint8_t *)malloc(record->length);
        if (buffer == NULL) return false;
        source_pc = ((uint32_t)(record->source_bank_active & 0x00FFu) << 16u) |
                    record->source_address;
        if (!read_source(context, source_pc, buffer, record->length)) {
            free(buffer);
            return false;
        }
        destination = ((size_t)record->vram_word_address * 2u) & 0xFFFFu;
        for (j = 0u; j < record->length; ++j)
            vram->bytes[(destination + j) & 0xFFFFu] = buffer[j];
        free(buffer);
        local.records++;
        local.bytes += record->length;
        record->source_bank_active = 0u;
    }
    if (stats != NULL) *stats = local;
    return true;
}
