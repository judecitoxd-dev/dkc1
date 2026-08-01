#ifndef DK1_BARREL_SCENE_RUNTIME_H
#define DK1_BARREL_SCENE_RUNTIME_H

#include <stdbool.h>
#include <stdint.h>
#include "dk1/barrel_live_runtime.h"
#include "dk1/object_animation_script.h"
#include "dk1/player_terrain_runtime.h"
#include "dk1/player_visual_runtime.h"
#include "dk1/player_preview_runtime.h"

typedef struct Dk1BarrelSceneRuntime {
    Dk1BarrelLiveRuntime live;
    Dk1ObjectAnimationScriptState animation;
    Dk1AnimationScriptResult animation_result;
    Dk1OamImage oam;
    Dk1VramImage vram;
    Dk1PlayerVisualStats visual;
    uint16_t selected_animation;
    uint32_t animation_resets;
    uint32_t animation_external_calls;
    bool ready;
    bool visual_ready;
} Dk1BarrelSceneRuntime;

bool dk1_barrel_scene_spawn(Dk1BarrelSceneRuntime *runtime,
                            const Dk1RomImage *rom,
                            size_t slot,
                            uint16_t type_id,
                            uint16_t world_x,
                            int16_t world_y);

bool dk1_barrel_scene_step(Dk1BarrelSceneRuntime *runtime,
                           const Dk1RomImage *rom,
                           Dk1PlayerTerrainRuntime *terrain,
                           const Dk1PlayerPreviewRuntime *player,
                           uint16_t pressed,
                           Dk1BarrelLiveResult *result);

bool dk1_barrel_scene_build_visual(Dk1BarrelSceneRuntime *runtime,
                                   const Dk1RomImage *rom,
                                   const Dk1VramImage *base_vram,
                                   uint16_t camera_x,
                                   uint16_t camera_y,
                                   uint16_t vertical_origin,
                                   uint8_t obsel);

#endif
