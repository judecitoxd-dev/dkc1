#ifndef DK1_BARREL_LIVE_RUNTIME_H
#define DK1_BARREL_LIVE_RUNTIME_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/barrel_runtime.h"
#include "dk1/player_motion.h"

typedef struct Dk1BarrelLiveTerrainOps {
    void *user_data;
    bool (*floor_crossing)(void *user_data,
                           uint16_t world_x,
                           int16_t old_bottom_y,
                           int16_t new_bottom_y,
                           int16_t *floor_y,
                           uint16_t *attributes);
    bool (*solid_point)(void *user_data,
                        int32_t world_x,
                        int32_t world_y,
                        uint16_t *attributes);
} Dk1BarrelLiveTerrainOps;

typedef struct Dk1BarrelLivePlayer {
    uint16_t world_x;
    int16_t world_y;
    int16_t velocity_x;
    int16_t velocity_y;
    bool facing_left;
    bool pickup_pressed;
    bool throw_pressed;
    bool holding_barrel;
} Dk1BarrelLivePlayer;

typedef enum Dk1BarrelLivePhase {
    DK1_BARREL_LIVE_INACTIVE = 0,
    DK1_BARREL_LIVE_IDLE,
    DK1_BARREL_LIVE_HELD,
    DK1_BARREL_LIVE_THROWN,
    DK1_BARREL_LIVE_ROLLING
} Dk1BarrelLivePhase;

typedef struct Dk1BarrelLiveRuntime {
    Dk1BarrelObject object;
    Dk1PlayerMotion motion;
    size_t slot;
    uint16_t animation_id;
    uint16_t floor_attributes;
    uint16_t wall_attributes;
    uint64_t frames;
    uint64_t dispatcher_steps;
    uint32_t pickups;
    uint32_t throws;
    uint32_t landings;
    uint32_t wall_hits;
    uint32_t breaks;
    uint32_t explosions;
    uint32_t kong_releases;
    Dk1BarrelLivePhase phase;
    bool active;
} Dk1BarrelLiveRuntime;

typedef struct Dk1BarrelLiveResult {
    Dk1BarrelRuntimeResult dispatcher;
    bool picked_up;
    bool thrown;
    bool landed;
    bool wall_hit;
    bool destroyed;
    bool exploded;
    bool released_kong;
    bool render_requested;
} Dk1BarrelLiveResult;

bool dk1_barrel_live_spawn(Dk1BarrelLiveRuntime *runtime,
                           size_t slot,
                           uint16_t type_id,
                           uint16_t world_x,
                           int16_t world_y);

bool dk1_barrel_live_step(Dk1BarrelLiveRuntime *runtime,
                          Dk1BarrelLivePlayer *player,
                          Dk1BarrelLiveTerrainOps terrain,
                          Dk1BarrelLiveResult *result);

#endif
