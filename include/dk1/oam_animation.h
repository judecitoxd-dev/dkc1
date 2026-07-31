#ifndef DK1_OAM_ANIMATION_H
#define DK1_OAM_ANIMATION_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/oam_image.h"

#define DK1_OAM_ANIMATION_MAX_FRAMES 64u

typedef struct Dk1OamAnimationFrame {
    uint16_t tile_number;
    uint8_t duration;
    int8_t x_offset;
    int8_t y_offset;
    bool horizontal_flip;
    bool vertical_flip;
} Dk1OamAnimationFrame;

typedef struct Dk1OamAnimation {
    Dk1OamAnimationFrame frames[DK1_OAM_ANIMATION_MAX_FRAMES];
    uint8_t frame_count;
    uint8_t frame_index;
    uint8_t tick;
    bool loop;
    bool finished;
} Dk1OamAnimation;

void dk1_oam_animation_init(Dk1OamAnimation *animation, bool loop);
bool dk1_oam_animation_add(Dk1OamAnimation *animation,
                           Dk1OamAnimationFrame frame);
bool dk1_oam_animation_step(Dk1OamAnimation *animation);
bool dk1_oam_animation_apply(const Dk1OamAnimation *animation,
                             Dk1OamImage *oam, uint8_t obsel,
                             size_t sprite_index, Dk1OamSprite base_sprite);

#endif
