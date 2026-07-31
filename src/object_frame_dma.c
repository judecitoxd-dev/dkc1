#include "dk1/object_frame_dma.h"
#include <string.h>

bool dk1_object_frame_dma_schedule(const Dk1ObjectFrameLayout *layout,
                                   uint16_t tile_base_number,
                                   Dk1NmiVramDmaQueue *queue,
                                   Dk1ObjectFrameDmaStats *stats) {
    Dk1ObjectFrameDmaStats local = {0};
    uint32_t source_pc;
    uint16_t destination;
    uint16_t length;
    if (layout == NULL || queue == NULL) return false;
    source_pc = layout->source_pc + 8u + (uint32_t)layout->piece_count * 2u;
    local.graphics_source_pc = source_pc;
    destination = (uint16_t)((tile_base_number & 0x01FFu) << 4u);
    length = (uint16_t)((uint16_t)layout->attribute_5 << 5u);
    if (length != 0u) {
        if (!dk1_nmi_vram_dma_push(queue, length, destination, source_pc)) return false;
        local.records++;
        local.bytes += length;
        source_pc += length;
    }
    length = (uint16_t)((uint16_t)(layout->attribute_7 & 0x0Fu) << 5u);
    if (length != 0u) {
        destination = (uint16_t)(destination + ((uint16_t)layout->attribute_6 << 4u));
        if (!dk1_nmi_vram_dma_push(queue, length, destination, source_pc)) return false;
        local.records++;
        local.bytes += length;
    }
    if (stats != NULL) *stats = local;
    return true;
}
