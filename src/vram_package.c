#include "dk1/vram_package.h"

bool dk1_vram_package_count(const Dk1RomImage *rom, size_t *count) {
    uint16_t first_offset = 0u;
    if (rom == NULL || count == NULL ||
        !dk1_rom_read_u16(rom, DK1_VRAM_PACKAGE_TABLE_SNES, &first_offset) ||
        first_offset == 0u || (first_offset & 1u) != 0u) return false;
    *count = first_offset / 2u;
    return true;
}

bool dk1_vram_package_read(const Dk1RomImage *rom, uint16_t package_id,
                           Dk1VramPackageRecord *records, size_t capacity,
                           size_t *record_count) {
    size_t package_count = 0u;
    uint16_t relative = 0u;
    uint32_t cursor = 0u;
    size_t count = 0u;
    if (rom == NULL || records == NULL || capacity == 0u || record_count == NULL ||
        !dk1_vram_package_count(rom, &package_count) || package_id >= package_count ||
        !dk1_rom_read_u16(rom,
            DK1_VRAM_PACKAGE_TABLE_SNES + (uint32_t)package_id * 2u,
            &relative)) return false;
    cursor = DK1_VRAM_PACKAGE_TABLE_SNES + relative;
    while (count < capacity && count < DK1_VRAM_PACKAGE_MAX_RECORDS) {
        uint8_t bank = 0u;
        uint16_t source = 0u;
        uint16_t destination = 0u;
        uint16_t transfer = 0u;
        if (!dk1_rom_read_u8(rom, cursor, &bank)) return false;
        if (bank == 0u) {
            *record_count = count;
            return true;
        }
        if (!dk1_rom_read_u16(rom, cursor + 1u, &source) ||
            !dk1_rom_read_u16(rom, cursor + 3u, &destination) ||
            !dk1_rom_read_u16(rom, cursor + 5u, &transfer)) return false;
        records[count].source_snes = ((uint32_t)bank << 16u) | source;
        records[count].destination_word = (uint16_t)(destination & 0x7FFFu);
        records[count].transfer_bytes = transfer;
        records[count].compressed = (destination & 0x8000u) != 0u;
        ++count;
        cursor += 7u;
    }
    return false;
}
