#include "dk1/object_actor_runtime.h"
#include <stdlib.h>
#include <string.h>

typedef struct Dk1ActorStepContext {
    Dk1ObjectActorRuntime *actor;
    const Dk1RomImage *rom;
    const Dk1VramImage *base_vram;
    Dk1ObjectScreenTransform transform;
    Dk1ObjectFrameOamStyle style;
    uint8_t obsel;
    bool success;
} Dk1ActorStepContext;

static bool overlaps(uint16_t ax, int16_t ay, int16_t ahw, int16_t ahh,
                     uint16_t bx, int16_t by, int16_t bhw, int16_t bhh) {
    int32_t dx = (int16_t)(uint16_t)(ax - bx);
    int32_t dy = (int32_t)ay - by;
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    return dx <= (int32_t)ahw + bhw && dy <= (int32_t)ahh + bhh;
}

static void actor_visit(size_t slot, Dk1ObjectPass pass, uint32_t callback_pc,
                        void *user_data) {
    Dk1ActorStepContext *context = (Dk1ActorStepContext *)user_data;
    Dk1ObjectActorRuntime *actor;
    (void)pass;
    if (context == NULL || context->actor == NULL ||
        slot != context->actor->slot) return;
    actor = context->actor;
    actor->scheduler_callbacks++;
    actor->callback_pc = callback_pc;
    if (callback_pc != DK1_ANIMATED_RENDER_CALLBACK_PC) {
        context->success = false;
        return;
    }
    actor->callback_verified = true;
    if (!dk1_object_animation_step(context->rom, &actor->animation,
                                   &actor->animation_result)) {
        context->success = false;
        return;
    }
    if (actor->animation.frame == 0u) return;
    context->transform.world_x = (int16_t)actor->world_x;
    context->transform.world_y = actor->world_y;
    actor->visual_ready = dk1_player_visual_build(
        context->rom, context->base_vram, actor->animation.frame,
        context->transform, context->style, context->obsel,
        &actor->oam, &actor->vram, &actor->visual);
    if (!actor->visual_ready) context->success = false;
}

bool dk1_object_actor_spawn(Dk1ObjectActorRuntime *actor,
                            const Dk1RomImage *rom,
                            size_t slot, uint16_t animation_id,
                            uint16_t world_x, int16_t world_y,
                            Dk1ActorInteractionMode interaction_mode) {
    Dk1ObjectTypeDispatch dispatch;
    if (actor == NULL || rom == NULL ||
        !dk1_object_slot_in_pool(slot, DK1_OBJECT_POOL_PRIMARY) ||
        !dk1_object_type_dispatch_get(DK1_ANIMATED_RENDER_OBJECT_TYPE, &dispatch) ||
        (((uint32_t)DK1_OBJECT_CALLBACK_BANK << 16u) |
         dispatch.callback_address_bf) != DK1_ANIMATED_RENDER_CALLBACK_PC) return false;
    memset(actor, 0, sizeof(*actor));
    actor->slot = slot;
    actor->type_id = DK1_ANIMATED_RENDER_OBJECT_TYPE;
    actor->world_x = world_x;
    actor->world_y = world_y;
    actor->half_width = 8;
    actor->half_height = 8;
    actor->interaction_mode = interaction_mode;
    actor->scheduler.type_id[slot] = actor->type_id;
    actor->animation.animation_id = animation_id;
    actor->animation.speed = 0x0100;
    if (!dk1_animation_script_pointer(rom, animation_id,
                                      &actor->animation.script_pointer)) return false;
    actor->active = true;
    return true;
}

bool dk1_object_actor_step(Dk1ObjectActorRuntime *actor,
                           const Dk1RomImage *rom,
                           const Dk1VramImage *base_vram,
                           Dk1ObjectScreenTransform transform,
                           Dk1ObjectFrameOamStyle style,
                           uint8_t obsel,
                           uint16_t player_x, int16_t player_y,
                           int16_t player_half_width,
                           int16_t player_half_height,
                           bool *interaction) {
    Dk1ActorStepContext context;
    if (interaction != NULL) *interaction = false;
    if (actor == NULL || rom == NULL || base_vram == NULL || !actor->active ||
        player_half_width < 0 || player_half_height < 0) return false;
    memset(&context, 0, sizeof(context));
    context.actor = actor;
    context.rom = rom;
    context.base_vram = base_vram;
    context.transform = transform;
    context.style = style;
    context.obsel = obsel;
    context.success = true;
    (void)dk1_object_run_frame(&actor->scheduler, actor_visit, &context);
    if (!context.success || !actor->callback_verified) return false;
    actor->frames++;
    if (actor->interaction_mode == DK1_ACTOR_INTERACTION_TOUCH_DEACTIVATE &&
        overlaps(actor->world_x, actor->world_y,
                 actor->half_width, actor->half_height,
                 player_x, player_y, player_half_width, player_half_height)) {
        actor->active = false;
        actor->scheduler.type_id[actor->slot] = 0u;
        actor->interactions++;
        if (interaction != NULL) *interaction = true;
    }
    return true;
}
