#ifndef DK1_VRAM_PACKAGE_H
#define DK1_VRAM_PACKAGE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/rom_image.h"

#define DK1_VRAM_PACKAGE_TABLE_SNES 0xB9A994u
#define DK1_VRAM_PACKAGE_MAX_RECORDS 16u

typedef struct Dk1VramPackageRecord {
    uint32_t source_snes;
    uint16_t destination_word;
    uint16_t transfer_bytes;
    bool compressed;
} Dk1VramPackageRecord;

/* Translation of the seven-byte DMA records consumed by $B9:A924. */
bool dk1_vram_package_count(const Dk1RomImage *rom, size_t *count);
bool dk1_vram_package_read(const Dk1RomImage *rom, uint16_t package_id,
                           Dk1VramPackageRecord *records, size_t capacity,
                           size_t *record_count);

#endif
