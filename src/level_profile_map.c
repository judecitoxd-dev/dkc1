#include "dk1/level_profile_map.h"
#include "dk1/level_dispatch.h"

static const uint8_t TERRAIN_PROFILES[DK1_LEVEL_PROFILE_MAP_COUNT] = {
    0u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 0u, 1u, 3u, 1u, 0u, 0u, 0u, 0u,
    1u, 1u, 1u, 1u, 1u, 1u, 0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
    0u, 0u, 5u, 1u, 7u, 0u, 1u, 2u, 7u, 1u, 5u, 2u, 2u, 3u, 9u, 2u,
    2u, 3u, 3u, 3u, 1u, 6u, 3u, 3u, 9u, 2u, 2u, 2u, 3u, 3u, 5u, 5u,
    4u, 4u, 6u, 6u, 4u, 4u, 1u, 1u, 1u, 3u, 3u, 3u, 1u, 1u, 2u, 2u,
    2u, 2u, 6u, 6u, 10u, 6u, 6u, 6u, 6u, 6u, 0u, 0u, 12u, 0u, 12u, 0u,
    4u, 4u, 8u, 4u, 4u, 11u, 7u, 7u, 12u, 7u, 7u, 7u, 0u, 8u, 0u, 0u,
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
    3u, 3u, 3u, 3u, 3u, 3u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 4u,
    4u, 4u, 4u, 4u, 7u, 7u, 7u, 1u, 1u, 1u, 1u, 1u, 1u, 2u, 2u, 2u,
    2u, 2u, 2u, 4u, 10u, 11u, 5u, 7u, 7u, 10u, 11u, 8u, 7u, 7u, 6u, 6u,
    6u, 6u, 6u, 10u, 10u, 10u, 10u, 11u, 11u, 11u, 11u, 11u, 11u, 2u, 4u, 5u,
    5u, 1u, 1u, 1u, 1u, 8u, 8u, 8u, 8u, 1u, 8u, 8u, 9u, 3u, 10u, 10u,
    11u, 11u, 7u, 1u, 11u, 10u, 10u, 10u, 10u, 2u, 2u, 11u, 11u, 11u, 5u, 5u,
    13u, 13u, 13u, 13u, 13u, 13u,
};

size_t dk1_level_profile_map_count(void) {
    return DK1_LEVEL_PROFILE_MAP_COUNT;
}

bool dk1_level_profile_map_get(uint16_t level_id,
                               Dk1LevelProfileMapEntry *entry) {
    Dk1LevelDispatchEntry dispatch;
    if (entry == NULL || level_id >= DK1_LEVEL_PROFILE_MAP_COUNT ||
        !dk1_level_dispatch_get(level_id, &dispatch)) return false;
    entry->init_callback_b9 = dispatch.bank_b9_callback_a;
    entry->terrain_profile = TERRAIN_PROFILES[level_id];
    return true;
}
