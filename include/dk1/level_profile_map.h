#ifndef DK1_LEVEL_PROFILE_MAP_H
#define DK1_LEVEL_PROFILE_MAP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DK1_LEVEL_PROFILE_MAP_COUNT 230u
#define DK1_LEVEL_TERRAIN_PROFILE_COUNT 14u

typedef struct Dk1LevelProfileMapEntry {
    uint16_t init_callback_b9;
    uint8_t terrain_profile;
} Dk1LevelProfileMapEntry;

/*
 * Address-only clean-room result obtained by following the bank-$B9 level-init
 * callback selected for each value of direct-page level id $003E. Each callback
 * reaches $81:8C66 with exactly one terrain/streaming profile id.
 */
size_t dk1_level_profile_map_count(void);
bool dk1_level_profile_map_get(uint16_t level_id,
                               Dk1LevelProfileMapEntry *entry);

#endif
