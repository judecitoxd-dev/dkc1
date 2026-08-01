#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "dk1/level_barrel_pool.h"

int main(void) {
    Dk1LevelBarrelPool pool;
    uint64_t populated_signature;
    size_t i;

    assert(dk1_level_barrel_type_supported(DK1_OBJECT_TYPE_STEEL_KEG));
    assert(dk1_level_barrel_type_supported(DK1_OBJECT_TYPE_BARREL));
    assert(dk1_level_barrel_type_supported(DK1_OBJECT_TYPE_ROPE_BARREL));
    assert(dk1_level_barrel_type_supported(DK1_OBJECT_TYPE_DK_BARREL));
    assert(dk1_level_barrel_type_supported(DK1_OBJECT_TYPE_TNT_BARREL));
    assert(!dk1_level_barrel_type_supported(DK1_OBJECT_TYPE_OIL_DRUM));
    assert(!dk1_level_barrel_type_supported(DK1_OBJECT_TYPE_GNAWTY));

    memset(&pool, 0, sizeof(pool));
    for (i = 0u; i < 3u; ++i) {
        Dk1LevelBarrelSlot *slot = &pool.slots[i];
        slot->runtime = (Dk1BarrelSceneRuntime *)calloc(
            1u, sizeof(*slot->runtime));
        assert(slot->runtime != NULL);
        slot->runtime->live.active = true;
        slot->runtime->live.phase = i == 1u ?
            DK1_BARREL_LIVE_HELD : DK1_BARREL_LIVE_IDLE;
        slot->record_index = 20u + i;
        slot->ready = true;
    }
    pool.active_count = 3u;
    pool.entered_count = 3u;
    pool.pickups = 1u;
    assert(dk1_level_barrel_pool_has_held(&pool));
    populated_signature = dk1_level_barrel_pool_signature(&pool);
    assert(populated_signature != 0u);

    dk1_level_barrel_pool_dispose(&pool);
    assert(pool.active_count == 0u);
    assert(pool.entered_count == 0u);
    assert(pool.overflow_count == 0u);
    assert(!dk1_level_barrel_pool_has_held(&pool));
    for (i = 0u; i < DK1_LEVEL_BARREL_POOL_CAPACITY; ++i) {
        assert(pool.slots[i].runtime == NULL);
        assert(pool.slots[i].record_index == 0u);
        assert(!pool.slots[i].ready);
    }

    dk1_level_barrel_pool_dispose(&pool);
    assert(pool.active_count == 0u);
    assert(dk1_level_barrel_pool_signature(&pool) != populated_signature);
    return 0;
}
