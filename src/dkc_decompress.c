#include "dk1/dkc_decompress.h"

#include <string.h>

static uint32_t bank_relative(uint32_t base, uint32_t delta) {
    return (base & 0xFF0000u) | ((base + delta) & 0xFFFFu);
}

static bool read8(const Dk1RomImage *rom, uint32_t base, uint32_t delta,
                  uint8_t *value) {
    return dk1_rom_read_u8(rom, bank_relative(base, delta), value);
}

static bool read16(const Dk1RomImage *rom, uint32_t base, uint32_t delta,
                   uint16_t *value) {
    uint8_t low = 0u;
    uint8_t high = 0u;
    if (!read8(rom, base, delta, &low) ||
        !read8(rom, base, delta + 1u, &high)) return false;
    *value = (uint16_t)(low | ((uint16_t)high << 8u));
    return true;
}

bool dk1_dkc_decompress(const Dk1RomImage *rom, uint32_t source_snes,
                        uint8_t *output, size_t output_capacity,
                        Dk1DecompressStats *stats) {
    Dk1DecompressStats local;
    uint32_t stream = 0x80u;
    size_t guard = 0u;
    if (rom == NULL || output == NULL || output_capacity == 0u) return false;
    memset(&local, 0, sizeof(local));
    while (guard++ < 0x100000u) {
        uint8_t command = 0u;
        uint8_t kind = 0u;
        uint8_t count = 0u;
        if (!read8(rom, source_snes, stream, &command)) return false;
        ++stream;
        ++local.stream_bytes_read;
        if (command == 0u) {
            if (stats != NULL) *stats = local;
            return true;
        }
        kind = (uint8_t)(command >> 6u);
        count = (uint8_t)(command & 0x3Fu);
        if (kind == 0u) {
            size_t i = 0u;
            if (local.bytes_written + command > output_capacity) return false;
            ++local.literal_commands;
            for (i = 0u; i < command; ++i) {
                if (!read8(rom, source_snes, stream, output + local.bytes_written)) {
                    return false;
                }
                ++stream;
                ++local.stream_bytes_read;
                ++local.bytes_written;
            }
        } else if (kind == 1u) {
            uint8_t value = 0u;
            if (count == 0u || local.bytes_written + count > output_capacity ||
                !read8(rom, source_snes, stream, &value)) return false;
            ++stream;
            ++local.stream_bytes_read;
            ++local.repeat_commands;
            memset(output + local.bytes_written, value, count);
            local.bytes_written += count;
        } else if (kind == 2u) {
            uint16_t source_offset = 0u;
            size_t i = 0u;
            if (count == 0u ||
                !read16(rom, source_snes, stream, &source_offset) ||
                source_offset >= local.bytes_written ||
                local.bytes_written + count > output_capacity) return false;
            stream += 2u;
            local.stream_bytes_read += 2u;
            ++local.copy_commands;
            for (i = 0u; i < count; ++i) {
                const size_t from = (size_t)source_offset + i;
                if (from >= local.bytes_written) return false;
                output[local.bytes_written++] = output[from];
            }
        } else {
            uint16_t word = 0u;
            if (local.bytes_written + 2u > output_capacity ||
                !read16(rom, source_snes, (uint32_t)count * 2u, &word)) {
                return false;
            }
            ++local.dictionary_commands;
            output[local.bytes_written++] = (uint8_t)word;
            output[local.bytes_written++] = (uint8_t)(word >> 8u);
        }
    }
    return false;
}
