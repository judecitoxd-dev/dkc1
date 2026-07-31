#ifndef DK1_OBJECT_ACTOR_RUNTIME_H
#define DK1_OBJECT_ACTOR_RUNTIME_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/object_animation_script.h"
#include "dk1/object_scheduler.h"
#include "dk1/player_visual_runtime.h"

/* Type $73 maps through $BF:817C to callback $BF:8453.  That callback
 * performs the translated animation step ($BE:80E1) and then the common
 * object render path ($BD:F503). */
#define DK1_ANIMATED_RENDER_OBJECT_TYPE 115u
#define DK1_ANIMATED_RENDER_CALLBACK_PC 0xBF8453u

/* Interaction modes below are portable host policies.  They are not claimed
 * to be the original type-$73 gameplay response. */
typedef enum Dk1ActorInteractionMode {
    DK1_ACTOR_INTERACTION_NONE = 0,
    DK1_ACTOR_INTERACTION_TOUCH_DEACTIVATE = 1
} Dk1ActorInteractionMode;

typedef struct Dk1ObjectActorRuntime {
    Dk1ObjectFrameState scheduler;
    Dk1ObjectAnimationScriptState animation;
    Dk1AnimationScriptResult animation_result;
    Dk1OamImage oam;
    Dk1VramImage vram;
    Dk1PlayerVisualStats visual;
    uint16_t world_x;
    int16_t world_y;
    int16_t half_width;
    int16_t half_height;
    uint16_t type_id;
    size_t slot;
    uint32_t callback_pc;
    uint64_t frames;
    uint32_t scheduler_callbacks;
    uint32_t interactions;
    Dk1ActorInteractionMode interaction_mode;
    bool active;
    bool visual_ready;
    bool callback_verified;
} Dk1ObjectActorRuntime;

bool dk1_object_actor_spawn(Dk1ObjectActorRuntime *actor,
                            const Dk1RomImage *rom,
                            size_t slot, uint16_t animation_id,
                            uint16_t world_x, int16_t world_y,
                            Dk1ActorInteractionMode interaction_mode);

bool dk1_object_actor_step(Dk1ObjectActorRuntime *actor,
                           const Dk1RomImage *rom,
                           const Dk1VramImage *base_vram,
                           Dk1ObjectScreenTransform transform,
                           Dk1ObjectFrameOamStyle style,
                           uint8_t obsel,
                           uint16_t player_x, int16_t player_y,
                           int16_t player_half_width,
                           int16_t player_half_height,
                           bool *interaction);

#endif
