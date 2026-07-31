#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "dk1/ppu_preset.h"

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
    Dk1PpuPreset preset;
    size_t count = 0u;
    uint8_t value = 0u;
    const uint32_t base = DK1_PPU_PRESET_TABLE_SNES;
    assert(bytes != NULL);
    assert(dk1_rom_image_init(&rom, bytes, DK1_EXPECTED_ROM_SIZE));

    put16(bytes, base, 4u);
    put16(bytes, base + 2u, 14u);
    put16(bytes, base + 4u, 0x2105u);
    bytes[offset(base + 6u)] = 0x09u;
    put16(bytes, base + 7u, 0xA10Bu);
    bytes[offset(base + 9u)] = 0x33u;
    bytes[offset(base + 10u)] = 0x02u;
    put16(bytes, base + 11u, 0u);
    put16(bytes, base + 14u, 0x2107u);
    bytes[offset(base + 16u)] = 0x6Du;
    put16(bytes, base + 17u, 0u);

    assert(dk1_ppu_preset_count(&rom, &count));
    assert(count == 2u);
    assert(dk1_ppu_preset_read(&rom, 0u, &preset));
    assert(preset.write_count == 3u);
    assert(dk1_ppu_preset_register(&preset, 0x2105u, &value) && value == 0x09u);
    assert(dk1_ppu_preset_register(&preset, 0x210Bu, &value) && value == 0x33u);
    assert(dk1_ppu_preset_register(&preset, 0x210Cu, &value) && value == 0x02u);
    assert(!dk1_ppu_preset_register(&preset, 0x2106u, &value));
    assert(dk1_ppu_preset_read(&rom, 1u, &preset));
    assert(dk1_ppu_preset_register(&preset, 0x2107u, &value) && value == 0x6Du);
    assert(!dk1_ppu_preset_read(&rom, 2u, &preset));

    free(bytes);
    return 0;
}
