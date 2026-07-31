#include <assert.h>
#include <stddef.h>
#include "dk1/level_profile_map.h"
#include "dk1/level_dispatch.h"

bool dk1_level_dispatch_get(uint16_t level_id, Dk1LevelDispatchEntry *entry) {
    if (entry == NULL || level_id >= 230u) return false;
    entry->bank_b9_callback_a = level_id == 0u ? 0x8590u :
        (level_id == 10u ? 0x890Fu : (level_id == 229u ? 0x8E68u : 0x8590u));
    return true;
}

int main(void) {
    Dk1LevelProfileMapEntry entry = {0};
    size_t counts[DK1_LEVEL_TERRAIN_PROFILE_COUNT] = {0};
    uint16_t level = 0u;

    assert(dk1_level_profile_map_count() == 230u);
    assert(dk1_level_profile_map_get(0u, &entry));
    assert(entry.init_callback_b9 == 0x8590u);
    assert(entry.terrain_profile == 0u);
    assert(dk1_level_profile_map_get(10u, &entry));
    assert(entry.init_callback_b9 == 0x890Fu);
    assert(entry.terrain_profile == 3u);
    assert(dk1_level_profile_map_get(229u, &entry));
    assert(entry.init_callback_b9 == 0x8E68u);
    assert(entry.terrain_profile == 13u);
    assert(!dk1_level_profile_map_get(230u, &entry));
    assert(!dk1_level_profile_map_get(0u, NULL));

    for (level = 0u; level < DK1_LEVEL_PROFILE_MAP_COUNT; ++level) {
        assert(dk1_level_profile_map_get(level, &entry));
        assert(entry.terrain_profile < DK1_LEVEL_TERRAIN_PROFILE_COUNT);
        ++counts[entry.terrain_profile];
    }
    for (level = 0u; level < DK1_LEVEL_TERRAIN_PROFILE_COUNT; ++level) {
        assert(counts[level] != 0u);
    }
    return 0;
}
