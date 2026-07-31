#ifndef DK1_PLAYER_TERRAIN_RUNTIME_H
#define DK1_PLAYER_TERRAIN_RUNTIME_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/player_preview_runtime.h"
#include "dk1/rom_terrain.h"

typedef struct Dk1PlayerTerrainRuntime {
    Dk1RomTerrainView view;
    uint16_t shape_flags[DK1_TERRAIN_SHAPE_COUNT];
    Dk1TerrainSample support;
    size_t column_count;
    int16_t foot_offset;
    int16_t half_width;
    int16_t max_step_height;
    uint16_t last_attributes;
    uint16_t wall_attributes;
    uint16_t ceiling_attributes;
    uint32_t resolved_contacts;
    uint32_t wall_contacts;
    uint32_t ceiling_contacts;
    uint32_t material_changes;
    uint32_t misses;
    bool blocked_left;
    bool blocked_right;
    bool hit_ceiling;
    bool ready;
} Dk1PlayerTerrainRuntime;

bool dk1_player_terrain_init(Dk1PlayerTerrainRuntime*,const Dk1RomImage*,Dk1LevelTerrainConfig,uint32_t);
bool dk1_player_terrain_place(Dk1PlayerTerrainRuntime*,Dk1PlayerPreviewRuntime*,uint16_t,uint16_t*);
void dk1_player_terrain_resolve(Dk1PlayerTerrainRuntime*,Dk1PlayerPreviewRuntime*,int16_t);
void dk1_player_terrain_resolve_motion(Dk1PlayerTerrainRuntime*,Dk1PlayerPreviewRuntime*,uint16_t,int16_t);
void dk1_player_terrain_step(Dk1PlayerTerrainRuntime*,Dk1PlayerPreviewRuntime*,uint16_t,uint16_t);

#endif
