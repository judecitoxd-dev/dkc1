#include "dk1/player_visual_runtime.h"
#include <string.h>

static bool read_rom(void *context, uint32_t source_pc,
                     uint8_t *output, size_t size) {
    return dk1_rom_copy((const Dk1RomImage *)context, source_pc, output, size);
}

bool dk1_player_visual_build(const Dk1RomImage *rom,
                             const Dk1VramImage *base_vram,
                             uint16_t frame_id,
                             Dk1ObjectScreenTransform transform,
                             Dk1ObjectFrameOamStyle style,
                             uint8_t obsel,
                             Dk1OamImage *oam,
                             Dk1VramImage *vram,
                             Dk1PlayerVisualStats *stats) {
    Dk1ObjectFrameLayout layout;
    Dk1ObjectScreenOamStats oam_stats;
    Dk1ObjectFrameDmaStats dma_stats;
    Dk1NmiVramDmaStats execute_stats;
    Dk1NmiVramDmaQueue queue;
    if (rom == NULL || base_vram == NULL || oam == NULL || vram == NULL || stats == NULL)
        return false;
    memset(stats, 0, sizeof(*stats));
    if (!dk1_object_frame_layout_read(rom, frame_id, &layout)) return false;
    *vram = *base_vram;
    dk1_oam_init(oam);
    if (!dk1_object_frame_emit_screen_oam(&layout, transform, style, obsel, 0u,
                                           oam, &oam_stats)) return false;
    dk1_nmi_vram_dma_init(&queue);
    if (!dk1_object_frame_dma_schedule(&layout, style.base_tile_number,
                                       &queue, &dma_stats) ||
        !dk1_nmi_vram_dma_execute(&queue, read_rom, (void *)rom, vram, &execute_stats))
        return false;
    if (dma_stats.records != execute_stats.records || dma_stats.bytes != execute_stats.bytes)
        return false;
    stats->frame_id = frame_id;
    stats->layout_signature = dk1_object_frame_layout_signature(&layout);
    stats->pieces = oam_stats.written;
    stats->dma_records = execute_stats.records;
    stats->dma_bytes = execute_stats.bytes;
    return true;
}
