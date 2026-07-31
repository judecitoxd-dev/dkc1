#ifndef DK1_DKC_DECOMPRESS_H
#define DK1_DKC_DECOMPRESS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/rom_image.h"

typedef struct Dk1DecompressStats {
    size_t bytes_written;
    size_t stream_bytes_read;
    size_t literal_commands;
    size_t repeat_commands;
    size_t copy_commands;
    size_t dictionary_commands;
} Dk1DecompressStats;

/*
 * Clean-room translation of $B8:982F-$B8:98C9.
 * source_snes points to a 128-byte dictionary. The command stream begins at
 * source_snes + $80 and terminates with command byte zero.
 */
bool dk1_dkc_decompress(const Dk1RomImage *rom, uint32_t source_snes,
                        uint8_t *output, size_t output_capacity,
                        Dk1DecompressStats *stats);

#endif
