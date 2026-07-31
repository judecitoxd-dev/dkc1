#ifndef DK1_PPU_PRESET_H
#define DK1_PPU_PRESET_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/rom_image.h"

#define DK1_PPU_PRESET_TABLE_SNES 0xB9A50Eu
#define DK1_PPU_REGISTER_FIRST 0x2100u
#define DK1_PPU_REGISTER_COUNT 0x40u

typedef struct Dk1PpuPreset {
    uint8_t values[DK1_PPU_REGISTER_COUNT];
    bool written[DK1_PPU_REGISTER_COUNT];
    size_t write_count;
} Dk1PpuPreset;

/* Translation of the compact register streams consumed by $B9:A4DC. */
bool dk1_ppu_preset_count(const Dk1RomImage *rom, size_t *count);
bool dk1_ppu_preset_read(const Dk1RomImage *rom, uint16_t preset_id,
                         Dk1PpuPreset *preset);
bool dk1_ppu_preset_register(const Dk1PpuPreset *preset, uint16_t address,
                             uint8_t *value);

#endif
