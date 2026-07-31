#ifndef DK1_ROM_IMAGE_H
#define DK1_ROM_IMAGE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DK1_EXPECTED_ROM_SIZE 0x400000u
#define DK1_HIROM_HEADER_OFFSET 0x00FFC0u

typedef struct Dk1RomImage {
    const uint8_t *data;
    size_t size;
} Dk1RomImage;

typedef struct Dk1OwnedRom {
    uint8_t *data;
    size_t size;
} Dk1OwnedRom;

typedef struct Dk1RomIdentity {
    char title[22];
    uint8_t map_mode;
    uint8_t cartridge_type;
    uint8_t rom_size_code;
    uint8_t sram_size_code;
    uint8_t region_code;
    uint8_t version;
    uint16_t checksum_complement;
    uint16_t checksum;
} Dk1RomIdentity;

bool dk1_rom_image_init(Dk1RomImage *image, const uint8_t *data, size_t size);
bool dk1_rom_load_file(const char *path, Dk1OwnedRom *owned, Dk1RomImage *image);
void dk1_rom_free(Dk1OwnedRom *owned);
bool dk1_hirom_offset(uint32_t snes_pc, size_t rom_size, size_t *offset);
bool dk1_rom_read_u8(const Dk1RomImage *image, uint32_t snes_pc, uint8_t *value);
bool dk1_rom_read_u16(const Dk1RomImage *image, uint32_t snes_pc, uint16_t *value);
bool dk1_rom_read_u24(const Dk1RomImage *image, uint32_t snes_pc, uint32_t *value);
bool dk1_rom_copy(const Dk1RomImage *image, uint32_t snes_pc, void *output, size_t size);
bool dk1_rom_identity(const Dk1RomImage *image, Dk1RomIdentity *identity);
bool dk1_rom_identity_is_supported_rev2(const Dk1RomIdentity *identity);

#endif
