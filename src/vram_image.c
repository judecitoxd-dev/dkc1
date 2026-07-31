#include "dk1/vram_image.h"

#include <string.h>

static uint32_t bank_relative(uint32_t base, size_t delta) {
    return (base & 0xFF0000u) | ((base + (uint32_t)delta) & 0xFFFFu);
}

static bool copy_direct(const Dk1RomImage *rom, uint32_t source_snes,
                        Dk1VramImage *image, uint16_t destination_word,
                        uint16_t transfer_bytes) {
    size_t i = 0u;
    size_t destination = (size_t)destination_word * 2u;
    for (i = 0u; i < transfer_bytes; ++i) {
        uint8_t value = 0u;
        const uint8_t bank = (uint8_t)(source_snes >> 16u);
        const uint16_t address = (uint16_t)(source_snes + (uint32_t)i);
        if (bank == 0x7Eu || bank == 0x7Fu) {
            const size_t wram_offset = ((size_t)(bank - 0x7Eu) << 16u) | address;
            value = image->wram[wram_offset];
        } else if (!dk1_rom_read_u8(rom, bank_relative(source_snes, i), &value)) {
            return false;
        }
        image->bytes[(destination + i) & (DK1_VRAM_BYTE_COUNT - 1u)] = value;
    }
    return true;
}

static void copy_scratch(Dk1VramImage *image, uint16_t destination_word,
                         uint16_t transfer_bytes) {
    size_t i = 0u;
    size_t destination = (size_t)destination_word * 2u;
    for (i = 0u; i < transfer_bytes; ++i) {
        image->bytes[(destination + i) & (DK1_VRAM_BYTE_COUNT - 1u)] =
            image->scratch[i % DK1_VRAM_SCRATCH_BYTE_COUNT];
    }
}

void dk1_vram_image_init(Dk1VramImage *image) {
    if (image != NULL) memset(image, 0, sizeof(*image));
}

bool dk1_vram_apply_package(const Dk1RomImage *rom, uint16_t package_id,
                            Dk1VramImage *image, Dk1VramLoadStats *stats) {
    Dk1VramPackageRecord records[DK1_VRAM_PACKAGE_MAX_RECORDS];
    Dk1VramLoadStats local;
    size_t record_count = 0u;
    size_t i = 0u;
    if (rom == NULL || image == NULL ||
        !dk1_vram_package_read(rom, package_id, records,
                               DK1_VRAM_PACKAGE_MAX_RECORDS, &record_count)) {
        return false;
    }
    memset(&local, 0, sizeof(local));
    for (i = 0u; i < record_count; ++i) {
        const Dk1VramPackageRecord *record = &records[i];
        ++local.records;
        local.dma_bytes += record->transfer_bytes;
        if (record->compressed) {
            Dk1DecompressStats decompressed;
            if (!dk1_dkc_decompress(rom, record->source_snes,
                                    image->scratch,
                                    DK1_VRAM_SCRATCH_BYTE_COUNT,
                                    &decompressed)) return false;
            ++local.compressed_records;
            local.decompressed_bytes += decompressed.bytes_written;
            copy_scratch(image, record->destination_word, record->transfer_bytes);
        } else {
            if (!copy_direct(rom, record->source_snes, image,
                             record->destination_word,
                             record->transfer_bytes)) return false;
            ++local.direct_records;
        }
    }
    if (stats != NULL) *stats = local;
    return true;
}
