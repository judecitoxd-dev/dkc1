#ifndef DK1_PLAYER_LIVE_RUNTIME_H
#define DK1_PLAYER_LIVE_RUNTIME_H
#include <stdbool.h>
#include <stdint.h>
#include "dk1/player_dispatch.h"
#include "dk1/player_preview_runtime.h"
#include "dk1/player_state_runtime.h"
#include "dk1/player_terrain_runtime.h"
typedef struct Dk1PlayerLiveRuntime {
    uint16_t state;
    uint32_t handler_pc;
    uint32_t required_calls;
    uint64_t dispatches;
    uint64_t local_steps;
    uint64_t plan_steps;
    bool used_local_semantics;
} Dk1PlayerLiveRuntime;
void dk1_player_live_init(Dk1PlayerLiveRuntime*);
void dk1_player_live_step(Dk1PlayerLiveRuntime*,Dk1PlayerPreviewRuntime*,Dk1PlayerTerrainRuntime*,uint16_t,uint16_t);
#endif
