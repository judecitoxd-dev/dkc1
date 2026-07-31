#include "dk1/ppu_preset.h"

#include <string.h>

static bool write_register(Dk1PpuPreset *preset, uint16_t address, uint8_t value) {
    const uint16_t index = (uint16_t)(address - DK1_PPU_REGISTER_FIRST);
    if (address < DK1_PPU_REGISTER_FIRST || index >= DK1_PPU_REGISTER_COUNT) {
        return false;
    }
    preset->values[index] = value;
    if (!preset->written[index]) {
        preset->written[index] = true;
        ++preset->write_count;
    }
    return true;
}

bool dk1_ppu_preset_count(const Dk1RomImage *rom, size_t *count) {
    uint16_t first_offset = 0u;
    if (rom == NULL || count == NULL ||
        !dk1_rom_read_u16(rom, DK1_PPU_PRESET_TABLE_SNES, &first_offset) ||
        first_offset == 0u || (first_offset & 1u) != 0u) return false;
    *count = first_offset / 2u;
    return true;
}

bool dk1_ppu_preset_read(const Dk1RomImage *rom, uint16_t preset_id,
                         Dk1PpuPreset *preset) {
    size_t count = 0u;
    uint16_t relative = 0u;
    uint32_t cursor = 0u;
    size_t guard = 0u;
    if (rom == NULL || preset == NULL ||
        !dk1_ppu_preset_count(rom, &count) || preset_id >= count ||
        !dk1_rom_read_u16(rom,
            DK1_PPU_PRESET_TABLE_SNES + (uint32_t)preset_id * 2u,
            &relative)) return false;
    memset(preset, 0, sizeof(*preset));
    cursor = DK1_PPU_PRESET_TABLE_SNES + relative;
    while (guard++ < 256u) {
        uint16_t encoded_address = 0u;
        uint16_t address = 0u;
        uint8_t first = 0u;
        uint8_t second = 0u;
        if (!dk1_rom_read_u16(rom, cursor, &encoded_address)) return false;
        cursor += 2u;
        if (encoded_address == 0u) return true;
        address = (uint16_t)(encoded_address & 0x7FFFu);
        if ((encoded_address & 0x8000u) != 0u) {
            if (!dk1_rom_read_u8(rom, cursor, &first) ||
                !dk1_rom_read_u8(rom, cursor + 1u, &second) ||
                !write_register(preset, address, first) ||
                !write_register(preset, (uint16_t)(address + 1u), second)) {
                return false;
            }
            cursor += 2u;
        } else {
            if (!dk1_rom_read_u8(rom, cursor, &second) ||
                !write_register(preset, address, second)) return false;
            cursor += 1u;
        }
    }
    return false;
}

bool dk1_ppu_preset_register(const Dk1PpuPreset *preset, uint16_t address,
                             uint8_t *value) {
    const uint16_t index = (uint16_t)(address - DK1_PPU_REGISTER_FIRST);
    if (preset == NULL || value == NULL || address < DK1_PPU_REGISTER_FIRST ||
        index >= DK1_PPU_REGISTER_COUNT || !preset->written[index]) return false;
    *value = preset->values[index];
    return true;
}
