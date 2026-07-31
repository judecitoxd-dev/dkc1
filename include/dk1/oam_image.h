#ifndef DK1_OAM_IMAGE_H
#define DK1_OAM_IMAGE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/vram_image.h"
#include "dk1/cgram_image.h"
#include "dk1/rgba_surface.h"

#define DK1_OAM_SPRITE_COUNT 128u
#define DK1_OAM_LOW_BYTES 512u
#define DK1_OAM_HIGH_BYTES 32u

typedef struct Dk1OamImage {
    uint8_t low[DK1_OAM_LOW_BYTES];
    uint8_t high[DK1_OAM_HIGH_BYTES];
} Dk1OamImage;

typedef struct Dk1OamSprite {
    int16_t x;
    uint8_t y;
    uint16_t tile_number;
    uint8_t palette;
    uint8_t priority;
    bool horizontal_flip;
    bool vertical_flip;
    bool large;
    uint8_t width;
    uint8_t height;
} Dk1OamSprite;

void dk1_oam_init(Dk1OamImage *oam);
bool dk1_oam_decode_sprite(const Dk1OamImage *oam, uint8_t obsel,
                           size_t sprite_index, Dk1OamSprite *sprite);
bool dk1_oam_encode_sprite(Dk1OamImage *oam, uint8_t obsel,
                           size_t sprite_index, const Dk1OamSprite *sprite);
uint16_t dk1_oam_character_base_word(uint8_t obsel, bool name_select);
bool dk1_oam_render(const Dk1OamImage *oam, uint8_t obsel,
                    const Dk1VramImage *vram, const Dk1CgramImage *cgram,
                    uint8_t minimum_priority, uint8_t maximum_priority,
                    Dk1RgbaSurface destination);

#endif
