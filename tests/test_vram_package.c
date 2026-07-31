#include <assert.h>
#include <stdlib.h>
#include "dk1/vram_package.h"

static size_t offset(uint32_t pc) {
    return ((size_t)((pc >> 16u) & 0x3Fu) << 16u) | (uint16_t)pc;
}
static void put16(uint8_t *rom, uint32_t pc, uint16_t value) {
    const size_t at = offset(pc);
    rom[at] = (uint8_t)value;
    rom[at + 1u] = (uint8_t)(value >> 8u);
}

int main(void) {
    uint8_t *bytes = (uint8_t *)calloc(DK1_EXPECTED_ROM_SIZE, 1u);
    Dk1RomImage rom;
    Dk1VramPackageRecord records[4];
    size_t count = 0u;
    const uint32_t base = DK1_VRAM_PACKAGE_TABLE_SNES;
    assert(bytes != NULL);
    assert(dk1_rom_image_init(&rom, bytes, DK1_EXPECTED_ROM_SIZE));

    put16(bytes, base, 4u);
    put16(bytes, base + 2u, 19u);
    bytes[offset(base + 4u)] = 0xD5u;
    put16(bytes, base + 5u, 0x9000u);
    put16(bytes, base + 7u, 0xB000u);
    put16(bytes, base + 9u, 0x0100u);
    bytes[offset(base + 11u)] = 0xCDu;
    put16(bytes, base + 12u, 0xF000u);
    put16(bytes, base + 14u, 0x2000u);
    put16(bytes, base + 16u, 0x0040u);
    bytes[offset(base + 18u)] = 0u;

    bytes[offset(base + 19u)] = 0xE0u;
    put16(bytes, base + 20u, 0x8123u);
    put16(bytes, base + 22u, 0x7FFFu);
    put16(bytes, base + 24u, 0x0002u);
    bytes[offset(base + 26u)] = 0u;

    assert(dk1_vram_package_count(&rom, &count) && count == 2u);
    assert(dk1_vram_package_read(&rom, 0u, records, 4u, &count));
    assert(count == 2u);
    assert(records[0].source_snes == 0xD59000u);
    assert(records[0].destination_word == 0x3000u);
    assert(records[0].transfer_bytes == 0x0100u);
    assert(records[0].compressed);
    assert(records[1].source_snes == 0xCDF000u);
    assert(!records[1].compressed);
    assert(dk1_vram_package_read(&rom, 1u, records, 4u, &count));
    assert(count == 1u && records[0].destination_word == 0x7FFFu);
    assert(!dk1_vram_package_read(&rom, 2u, records, 4u, &count));
    assert(!dk1_vram_package_read(&rom, 0u, records, 1u, &count));

    free(bytes);
    return 0;
}
