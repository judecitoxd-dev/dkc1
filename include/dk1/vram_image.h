#ifndef DK1_VRAM_IMAGE_H
#define DK1_VRAM_IMAGE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/dkc_decompress.h"
#include "dk1/vram_package.h"

#define DK1_VRAM_BYTE_COUNT 0x10000u
#define DK1_VRAM_SCRATCH_BYTE_COUNT 0x8000u
#define DK1_WRAM_BYTE_COUNT 0x20000u

typedef struct Dk1VramImage {
    uint8_t bytes[DK1_VRAM_BYTE_COUNT];
    uint8_t scratch[DK1_VRAM_SCRATCH_BYTE_COUNT];
    uint8_t wram[DK1_WRAM_BYTE_COUNT];
} Dk1VramImage;

typedef struct Dk1VramLoadStats {
    size_t records;
    size_t direct_records;
    size_t compressed_records;
    size_t dma_bytes;
    size_t decompressed_bytes;
} Dk1VramLoadStats;

void dk1_vram_image_init(Dk1VramImage *image);
bool dk1_vram_apply_package(const Dk1RomImage *rom, uint16_t package_id,
                            Dk1VramImage *image, Dk1VramLoadStats *stats);

#endif
