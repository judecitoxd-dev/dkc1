#include "dk1/oam_animation.h"
#include <string.h>

void dk1_oam_animation_init(Dk1OamAnimation *animation, bool loop) {
    if (animation == NULL) return;
    memset(animation, 0, sizeof(*animation));
    animation->loop = loop;
}

bool dk1_oam_animation_add(Dk1OamAnimation *animation,
                           Dk1OamAnimationFrame frame) {
    if (animation == NULL || animation->frame_count >= DK1_OAM_ANIMATION_MAX_FRAMES ||
        frame.duration == 0u || frame.tile_number > 0x1FFu) return false;
    animation->frames[animation->frame_count++] = frame;
    return true;
}

bool dk1_oam_animation_step(Dk1OamAnimation *animation) {
    if (animation == NULL || animation->frame_count == 0u || animation->finished) return false;
    ++animation->tick;
    if (animation->tick < animation->frames[animation->frame_index].duration) return true;
    animation->tick = 0u;
    if ((uint8_t)(animation->frame_index + 1u) < animation->frame_count) {
        ++animation->frame_index;
    } else if (animation->loop) {
        animation->frame_index = 0u;
    } else {
        animation->finished = true;
    }
    return true;
}

bool dk1_oam_animation_apply(const Dk1OamAnimation *animation,
                             Dk1OamImage *oam, uint8_t obsel,
                             size_t sprite_index, Dk1OamSprite base_sprite) {
    const Dk1OamAnimationFrame *frame;
    if (animation == NULL || oam == NULL || animation->frame_count == 0u ||
        animation->frame_index >= animation->frame_count) return false;
    frame = &animation->frames[animation->frame_index];
    base_sprite.x = (int16_t)(base_sprite.x + frame->x_offset);
    base_sprite.y = (uint8_t)(base_sprite.y + frame->y_offset);
    base_sprite.tile_number = frame->tile_number;
    base_sprite.horizontal_flip ^= frame->horizontal_flip;
    base_sprite.vertical_flip ^= frame->vertical_flip;
    return dk1_oam_encode_sprite(oam, obsel, sprite_index, &base_sprite);
}
