#ifndef DK1_LEVEL_DISPATCH_H
#define DK1_LEVEL_DISPATCH_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DK1_LEVEL_DISPATCH_COUNT 230u

typedef struct Dk1LevelDispatchEntry {
    uint16_t pre_frame_callback_80;
    uint16_t frame_callback_80;
    uint16_t entry_callback_80;
    uint16_t bank_b9_callback_a;
    uint16_t bank_b9_callback_b;
    uint16_t bank_b9_callback_c;
} Dk1LevelDispatchEntry;

/*
 * Address-only clean-room map for the level id in direct-page RAM $003E.
 * Sources: $80:C56C, $BF:FDC8, $B9:801E, $B9:81EA, $B9:83B6.
 */
size_t dk1_level_dispatch_count(void);
bool dk1_level_dispatch_get(uint16_t level_id, Dk1LevelDispatchEntry *entry);

#endif
