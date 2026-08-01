#include <assert.h>
#include <string.h>
#include "dk1/barrel_live_runtime.h"

typedef struct TestTerrain {
    int16_t floor_y;
    int32_t wall_x;
    uint16_t floor_attr;
    uint16_t wall_attr;
} TestTerrain;

static bool floor_crossing(void *user_data,
                           uint16_t world_x,
                           int16_t old_bottom,
                           int16_t new_bottom,
                           int16_t *floor_y,
                           uint16_t *attributes) {
    TestTerrain *terrain = (TestTerrain *)user_data;
    (void)world_x;
    if (old_bottom >= terrain->floor_y && new_bottom <= terrain->floor_y) {
        *floor_y = terrain->floor_y;
        *attributes = terrain->floor_attr;
        return true;
    }
    return false;
}

static bool solid_point(void *user_data,
                        int32_t world_x,
                        int32_t world_y,
                        uint16_t *attributes) {
    TestTerrain *terrain = (TestTerrain *)user_data;
    (void)world_y;
    if (world_x >= terrain->wall_x) {
        *attributes = terrain->wall_attr;
        return true;
    }
    return false;
}

static Dk1BarrelLiveTerrainOps terrain_ops(TestTerrain *terrain) {
    Dk1BarrelLiveTerrainOps ops = {
        terrain,
        floor_crossing,
        solid_point
    };
    return ops;
}

static void pickup_and_throw(Dk1BarrelLiveRuntime *runtime,
                             Dk1BarrelLivePlayer *player,
                             Dk1BarrelLiveTerrainOps terrain) {
    Dk1BarrelLiveResult result;
    player->pickup_pressed = true;
    assert(dk1_barrel_live_step(runtime, player, terrain, &result));
    assert(result.picked_up);
    assert(runtime->phase == DK1_BARREL_LIVE_HELD);
    assert(player->holding_barrel);

    player->pickup_pressed = false;
    player->world_x = (uint16_t)(player->world_x + 10u);
    assert(dk1_barrel_live_step(runtime, player, terrain, &result));
    assert(runtime->motion.world_x == (uint16_t)(player->world_x + 12u));

    player->throw_pressed = true;
    assert(dk1_barrel_live_step(runtime, player, terrain, &result));
    assert(result.thrown);
    assert(runtime->phase == DK1_BARREL_LIVE_THROWN);
    assert(!player->holding_barrel);
    player->throw_pressed = false;
}

int main(void) {
    TestTerrain terrain = {0, 150, 0x0040u, 0x0080u};
    Dk1BarrelLiveRuntime runtime;
    Dk1BarrelLivePlayer player;
    Dk1BarrelLiveResult result;
    unsigned i;

    memset(&player, 0, sizeof(player));
    player.world_x = 100u;
    player.world_y = 8;
    assert(dk1_barrel_live_spawn(&runtime, 3u,
                                 DK1_OBJECT_TYPE_BARREL, 100u, 8));
    assert(runtime.animation_id == DK1_ANIMATION_BARREL_IDLE);
    assert(runtime.phase == DK1_BARREL_LIVE_IDLE);
    pickup_and_throw(&runtime, &player, terrain_ops(&terrain));
    for (i = 0u; i < 80u && runtime.active; ++i)
        assert(dk1_barrel_live_step(&runtime, &player,
                                    terrain_ops(&terrain), &result));
    assert(!runtime.active);
    assert(runtime.breaks == 1u);
    assert(runtime.wall_hits == 1u);
    assert(result.destroyed && !result.exploded);
    assert(result.dispatcher.sound_id == DK1_SOUND_BREAK_BARREL);
    assert(runtime.pickups == 1u && runtime.throws == 1u);
    assert(runtime.dispatcher_steps >= 4u);

    memset(&player, 0, sizeof(player));
    player.world_x = 100u;
    player.world_y = 8;
    assert(dk1_barrel_live_spawn(&runtime, 4u,
                                 DK1_OBJECT_TYPE_STEEL_KEG, 100u, 8));
    pickup_and_throw(&runtime, &player, terrain_ops(&terrain));
    for (i = 0u; i < 80u && runtime.wall_hits == 0u; ++i)
        assert(dk1_barrel_live_step(&runtime, &player,
                                    terrain_ops(&terrain), &result));
    assert(runtime.active);
    assert(runtime.wall_hits == 1u);
    assert(runtime.motion.velocity_x < 0);
    assert(runtime.breaks == 0u);

    terrain.wall_x = 10000;
    memset(&player, 0, sizeof(player));
    player.world_x = 100u;
    player.world_y = 8;
    assert(dk1_barrel_live_spawn(&runtime, 5u,
                                 DK1_OBJECT_TYPE_TNT_BARREL, 100u, 8));
    pickup_and_throw(&runtime, &player, terrain_ops(&terrain));
    for (i = 0u; i < 160u && runtime.active; ++i)
        assert(dk1_barrel_live_step(&runtime, &player,
                                    terrain_ops(&terrain), &result));
    assert(!runtime.active);
    assert(runtime.landings == 1u);
    assert(runtime.explosions == 1u);
    assert(result.exploded);
    assert(result.dispatcher.spawn_script_count == 2u);

    memset(&player, 0, sizeof(player));
    player.world_x = 100u;
    player.world_y = 8;
    assert(dk1_barrel_live_spawn(&runtime, 6u,
                                 DK1_OBJECT_TYPE_DK_BARREL, 100u, 8));
    pickup_and_throw(&runtime, &player, terrain_ops(&terrain));
    for (i = 0u; i < 160u && runtime.active; ++i)
        assert(dk1_barrel_live_step(&runtime, &player,
                                    terrain_ops(&terrain), &result));
    assert(!runtime.active);
    assert(runtime.kong_releases == 1u);
    assert(result.released_kong);

    assert(!dk1_barrel_live_spawn(&runtime, 7u,
                                  DK1_OBJECT_TYPE_OIL_DRUM, 100u, 8));
    return 0;
}
