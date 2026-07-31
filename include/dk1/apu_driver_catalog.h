#ifndef DK1_APU_DRIVER_CATALOG_H
#define DK1_APU_DRIVER_CATALOG_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/rom_image.h"

#define DK1_APU_DRIVER_SOURCE_TABLE 0x8AB15Eu
#define DK1_APU_DRIVER_SOURCE_COUNT 27u
#define DK1_SPC_RAM_SIZE 0x10000u

typedef struct Dk1ApuDriverSource {
    uint16_t address;
    uint8_t bank;
    uint32_t source_pc;
} Dk1ApuDriverSource;

typedef struct Dk1ApuUploadBlock {
    uint32_t source_pc;
    uint32_t data_pc;
    uint16_t length;
    uint16_t target;
} Dk1ApuUploadBlock;

size_t dk1_apu_driver_source_count(void);
bool dk1_apu_driver_source_get(const Dk1RomImage *rom, uint16_t index,
                               Dk1ApuDriverSource *source);
bool dk1_apu_upload_block_get(const Dk1RomImage *rom, uint16_t index,
                              Dk1ApuUploadBlock *block);
bool dk1_apu_upload_block_load(const Dk1RomImage *rom,
                               const Dk1ApuUploadBlock *block,
                               uint8_t spc_ram[DK1_SPC_RAM_SIZE]);
bool dk1_apu_driver_catalog_validate(const Dk1RomImage *rom, size_t *valid_count,
                                     size_t *payload_bytes);
uint64_t dk1_apu_driver_catalog_signature(const Dk1RomImage *rom);
uint64_t dk1_apu_driver_payload_signature(const Dk1RomImage *rom);

#endif
