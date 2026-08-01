#ifndef DK1_GNAWTY_RUNTIME_H
#define DK1_GNAWTY_RUNTIME_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/object_animation_script.h"
#include "dk1/object_identity.h"
#include "dk1/object_scheduler.h"
#include "dk1/player_motion.h"
#include "dk1/player_preview_runtime.h"
#include "dk1/player_terrain_runtime.h"
#include "dk1/player_visual_runtime.h"

#define DK1_GNAWTY_HALF_WIDTH 12
#define DK1_GNAWTY_HALF_HEIGHT 10
#define DK1_GNAWTY_WALK_SPEED 0x0080
#define DK1_GNAWTY_STOMP_BOUNCE 0x0400
#define DK1_GNAWTY_DEFEAT_FRAMES 24u

typedef enum Dk1GnawtyPhase {
    DK1_GNAWTY_WALKING = 0,
    DK1_GNAWTY_DEFEATED
} Dk1GnawtyPhase;

typedef struct Dk1GnawtyStepResult {
    bool turned;
    bool wall_hit;
    bool ledge_hit;
    bool stomped;
    bool player_hurt;
    bool render_requested;
} Dk1GnawtyStepResult;

typedef struct Dk1GnawtyRuntime {
    Dk1ObjectFrameState scheduler;
    Dk1PlayerMotion motion;
    Dk1ObjectAnimationScriptState animation;
    Dk1AnimationScriptResult animation_result;
    Dk1OamImage oam;
    Dk1VramImage vram;
    Dk1PlayerVisualStats visual;
    uint16_t patrol_left;
    uint16_t patrol_right;
    uint16_t type_id;
    size_t slot;
    uint32_t callback_pc;
    uint64_t frames;
    uint32_t scheduler_callbacks;
    uint32_t turns;
    uint32_t wall_hits;
    uint32_t ledge_hits;
    uint32_t stomps;
    uint32_t player_contacts;
    uint16_t defeat_timer;
    Dk1GnawtyPhase phase;
    bool facing_left;
    bool active;
    bool visual_ready;
    bool callback_verified;
} Dk1GnawtyRuntime;

/*
 * The object type, callback and animation IDs are taken from the original
 * tables. Patrol bounds and host collision policy remain a portable bridge
 * until callback $BF:840C and the shared enemy helpers are translated fully.
 */
bool dk1_gnawty_spawn(Dk1GnawtyRuntime *runtime,
                       const Dk1RomImage *rom,
                       size_t slot,
                       uint16_t world_x,
                       int16_t world_y,
                       bool facing_left);

bool dk1_gnawty_step(Dk1GnawtyRuntime *runtime,
                      const Dk1RomImage *rom,
                      const Dk1PlayerTerrainRuntime *terrain,
                      Dk1PlayerPreviewRuntime *player,
                      Dk1GnawtyStepResult *result);

bool dk1_gnawty_build_visual(Dk1GnawtyRuntime *runtime,
                              const Dk1RomImage *rom,
                              const Dk1VramImage *base_vram,
                              uint16_t camera_x,
                              uint16_t camera_y,
                              uint16_t vertical_origin,
                              uint8_t obsel);

#endif
