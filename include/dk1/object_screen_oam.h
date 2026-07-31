#ifndef DK1_OBJECT_SCREEN_OAM_H
#define DK1_OBJECT_SCREEN_OAM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/object_frame_layout.h"

typedef struct Dk1ObjectScreenTransform {
    int16_t world_x;
    int16_t world_y;
    int16_t camera_x;
    int16_t vertical_origin;
    int16_t vertical_scroll;
    int16_t vertical_offset;
    uint16_t object_flags;
} Dk1ObjectScreenTransform;

typedef struct Dk1ObjectScreenOamStats {
    size_t considered;
    size_t written;
    size_t vertically_clipped;
} Dk1ObjectScreenOamStats;

bool dk1_object_frame_emit_screen_oam(const Dk1ObjectFrameLayout *layout,
                                      Dk1ObjectScreenTransform transform,
                                      Dk1ObjectFrameOamStyle style,
                                      uint8_t obsel,
                                      size_t first_sprite,
                                      Dk1OamImage *oam,
                                      Dk1ObjectScreenOamStats *stats);

#endif
