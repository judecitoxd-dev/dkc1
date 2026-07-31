#ifndef DK1_PLAYER_VISUAL_RUNTIME_H
#define DK1_PLAYER_VISUAL_RUNTIME_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/nmi_vram_dma.h"
#include "dk1/object_frame_dma.h"
#include "dk1/object_screen_oam.h"
#include "dk1/rom_image.h"

typedef struct Dk1PlayerVisualStats {
    uint16_t frame_id;
    uint64_t layout_signature;
    size_t pieces;
    size_t dma_records;
    size_t dma_bytes;
} Dk1PlayerVisualStats;

bool dk1_player_visual_build(const Dk1RomImage *rom,
                             const Dk1VramImage *base_vram,
                             uint16_t frame_id,
                             Dk1ObjectScreenTransform transform,
                             Dk1ObjectFrameOamStyle style,
                             uint8_t obsel,
                             Dk1OamImage *oam,
                             Dk1VramImage *vram,
                             Dk1PlayerVisualStats *stats);

#endif
