#include <assert.h>
#include <stdlib.h>
#include "dk1/vram_image.h"

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
    Dk1VramImage *vram = (Dk1VramImage *)malloc(sizeof(*vram));
    Dk1VramLoadStats stats;
    Dk1RomImage rom;
    const uint32_t table = DK1_VRAM_PACKAGE_TABLE_SNES;
    const uint32_t compressed = 0xC09000u;
    assert(bytes != NULL && vram != NULL);
    assert(dk1_rom_image_init(&rom, bytes, DK1_EXPECTED_ROM_SIZE));

    put16(bytes, table, 2u);
    bytes[offset(table + 2u)] = 0xC0u;
    put16(bytes, table + 3u, 0x9000u);
    put16(bytes, table + 5u, 0x8001u);
    put16(bytes, table + 7u, 8u);
    bytes[offset(table + 9u)] = 0xC0u;
    put16(bytes, table + 10u, 0xA000u);
    put16(bytes, table + 12u, 5u);
    put16(bytes, table + 14u, 3u);
    bytes[offset(table + 16u)] = 0u;

    bytes[offset(compressed + 0x80u)] = 3u;
    bytes[offset(compressed + 0x81u)] = 1u;
    bytes[offset(compressed + 0x82u)] = 2u;
    bytes[offset(compressed + 0x83u)] = 3u;
    bytes[offset(compressed + 0x84u)] = 0u;
    bytes[offset(0xC0A000u)] = 7u;
    bytes[offset(0xC0A001u)] = 8u;
    bytes[offset(0xC0A002u)] = 9u;

    dk1_vram_image_init(vram);
    assert(dk1_vram_apply_package(&rom, 0u, vram, &stats));
    assert(stats.records == 2u);
    assert(stats.compressed_records == 1u);
    assert(stats.direct_records == 1u);
    assert(stats.dma_bytes == 11u);
    assert(stats.decompressed_bytes == 3u);
    assert(vram->bytes[2u] == 1u && vram->bytes[3u] == 2u &&
           vram->bytes[4u] == 3u && vram->bytes[5u] == 0u);
    assert(vram->bytes[10u] == 7u && vram->bytes[11u] == 8u &&
           vram->bytes[12u] == 9u);

    free(vram);
    free(bytes);
    return 0;
}
