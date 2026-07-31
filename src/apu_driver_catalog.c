#include "dk1/apu_driver_catalog.h"
#include <string.h>

static uint64_t hash_byte(uint64_t hash, uint8_t byte) {
    hash ^= byte;
    return hash * 1099511628211ull;
}

size_t dk1_apu_driver_source_count(void) { return DK1_APU_DRIVER_SOURCE_COUNT; }

bool dk1_apu_driver_source_get(const Dk1RomImage *rom, uint16_t index,
                               Dk1ApuDriverSource *source) {
    uint32_t entry_pc;
    uint16_t address;
    uint8_t bank;
    size_t offset;
    if (rom == NULL || source == NULL || index >= DK1_APU_DRIVER_SOURCE_COUNT) return false;
    entry_pc = DK1_APU_DRIVER_SOURCE_TABLE + (uint32_t)index * 3u;
    if (!dk1_rom_read_u16(rom, entry_pc, &address) ||
        !dk1_rom_read_u8(rom, entry_pc + 2u, &bank)) return false;
    source->address = address;
    source->bank = bank;
    source->source_pc = ((uint32_t)bank << 16u) | address;
    return dk1_hirom_offset(source->source_pc, rom->size, &offset);
}

bool dk1_apu_upload_block_get(const Dk1RomImage *rom, uint16_t index,
                              Dk1ApuUploadBlock *block) {
    Dk1ApuDriverSource source;
    size_t data_offset;
    if (rom == NULL || block == NULL ||
        !dk1_apu_driver_source_get(rom, index, &source) ||
        !dk1_rom_read_u16(rom, source.source_pc, &block->length) ||
        !dk1_rom_read_u16(rom, source.source_pc + 2u, &block->target)) return false;
    block->source_pc = source.source_pc;
    block->data_pc = source.source_pc + 4u;
    if (block->length == 0u || (uint32_t)block->target + block->length > DK1_SPC_RAM_SIZE)
        return false;
    return dk1_hirom_offset(block->data_pc, rom->size, &data_offset) &&
           data_offset + block->length <= rom->size;
}

bool dk1_apu_upload_block_load(const Dk1RomImage *rom,
                               const Dk1ApuUploadBlock *block,
                               uint8_t spc_ram[DK1_SPC_RAM_SIZE]) {
    if (rom == NULL || block == NULL || spc_ram == NULL ||
        (uint32_t)block->target + block->length > DK1_SPC_RAM_SIZE) return false;
    return dk1_rom_copy(rom, block->data_pc, spc_ram + block->target, block->length);
}

bool dk1_apu_driver_catalog_validate(const Dk1RomImage *rom, size_t *valid_count,
                                     size_t *payload_bytes) {
    size_t count = 0u, bytes = 0u;
    uint16_t i;
    Dk1ApuUploadBlock block;
    if (rom == NULL) return false;
    for (i = 0u; i < DK1_APU_DRIVER_SOURCE_COUNT; ++i) {
        if (!dk1_apu_upload_block_get(rom, i, &block)) {
            if (valid_count != NULL) *valid_count = count;
            if (payload_bytes != NULL) *payload_bytes = bytes;
            return false;
        }
        ++count;
        bytes += block.length;
    }
    if (valid_count != NULL) *valid_count = count;
    if (payload_bytes != NULL) *payload_bytes = bytes;
    return true;
}

uint64_t dk1_apu_driver_catalog_signature(const Dk1RomImage *rom) {
    uint64_t hash = 14695981039346656037ull;
    uint16_t i;
    for (i = 0u; i < DK1_APU_DRIVER_SOURCE_COUNT; ++i) {
        Dk1ApuDriverSource source;
        unsigned b;
        if (!dk1_apu_driver_source_get(rom, i, &source)) return 0u;
        for (b = 0u; b < 4u; ++b) hash = hash_byte(hash, (uint8_t)(source.source_pc >> (b * 8u)));
    }
    return hash;
}

uint64_t dk1_apu_driver_payload_signature(const Dk1RomImage *rom) {
    uint64_t hash = 14695981039346656037ull;
    uint16_t i;
    uint8_t buffer[256];
    for (i = 0u; i < DK1_APU_DRIVER_SOURCE_COUNT; ++i) {
        Dk1ApuUploadBlock block;
        size_t remaining, offset = 0u;
        unsigned b;
        if (!dk1_apu_upload_block_get(rom, i, &block)) return 0u;
        for (b = 0u; b < 4u; ++b) hash = hash_byte(hash, (uint8_t)(block.source_pc >> (b * 8u)));
        for (b = 0u; b < 2u; ++b) hash = hash_byte(hash, (uint8_t)(block.length >> (b * 8u)));
        for (b = 0u; b < 2u; ++b) hash = hash_byte(hash, (uint8_t)(block.target >> (b * 8u)));
        remaining = block.length;
        while (remaining != 0u) {
            size_t chunk = remaining < sizeof(buffer) ? remaining : sizeof(buffer);
            if (!dk1_rom_copy(rom, block.data_pc + (uint32_t)offset, buffer, chunk)) return 0u;
            for (b = 0u; b < chunk; ++b) hash = hash_byte(hash, buffer[b]);
            offset += chunk;
            remaining -= chunk;
        }
    }
    return hash;
}
