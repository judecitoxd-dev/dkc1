#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "dk1/dkc_decompress.h"

static size_t offset(uint32_t pc) {
    return ((size_t)((pc >> 16u) & 0x3Fu) << 16u) | (uint16_t)pc;
}

int main(void) {
    uint8_t *bytes = (uint8_t *)calloc(DK1_EXPECTED_ROM_SIZE, 1u);
    uint8_t output[32] = {0};
    const uint8_t expected[] = {1u,2u,3u,9u,9u,9u,9u,2u,3u,9u,0xAAu,0xBBu};
    Dk1RomImage rom;
    Dk1DecompressStats stats;
    const uint32_t source = 0xC09000u;
    size_t at = 0u;
    assert(bytes != NULL);
    assert(dk1_rom_image_init(&rom, bytes, DK1_EXPECTED_ROM_SIZE));

    bytes[offset(source + 6u)] = 0xAAu;
    bytes[offset(source + 7u)] = 0xBBu;
    at = offset(source + 0x80u);
    bytes[at++] = 0x03u; bytes[at++] = 1u; bytes[at++] = 2u; bytes[at++] = 3u;
    bytes[at++] = 0x44u; bytes[at++] = 9u;
    bytes[at++] = 0x83u; bytes[at++] = 1u; bytes[at++] = 0u;
    bytes[at++] = 0xC3u;
    bytes[at++] = 0u;

    assert(dk1_dkc_decompress(&rom, source, output, sizeof(output), &stats));
    assert(stats.bytes_written == sizeof(expected));
    assert(stats.literal_commands == 1u);
    assert(stats.repeat_commands == 1u);
    assert(stats.copy_commands == 1u);
    assert(stats.dictionary_commands == 1u);
    assert(memcmp(output, expected, sizeof(expected)) == 0);
    assert(!dk1_dkc_decompress(&rom, source, output, 4u, &stats));

    memset(bytes + offset(source + 0x80u), 0, 16u);
    bytes[offset(source + 0x80u)] = 0x81u;
    bytes[offset(source + 0x81u)] = 0u;
    bytes[offset(source + 0x82u)] = 0u;
    assert(!dk1_dkc_decompress(&rom, source, output, sizeof(output), &stats));

    free(bytes);
    return 0;
}
