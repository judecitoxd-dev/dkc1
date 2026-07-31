#ifndef DK1_OBJECT_FRAME_LAYOUT_H
#define DK1_OBJECT_FRAME_LAYOUT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/oam_image.h"
#include "dk1/rom_image.h"

#define DK1_OBJECT_FRAME_POINTER_TABLE 0xBBCC9Cu
#define DK1_OBJECT_FRAME_MAX_PIECES 64u

typedef enum Dk1ObjectFramePieceGroup {
    DK1_OBJECT_FRAME_LARGE = 0,
    DK1_OBJECT_FRAME_SMALL = 1,
    DK1_OBJECT_FRAME_AUXILIARY = 2
} Dk1ObjectFramePieceGroup;

typedef struct Dk1ObjectFramePiece {
    uint8_t encoded_x;
    uint8_t encoded_y;
    int16_t x_offset;
    int16_t y_offset;
    uint16_t tile_offset;
    Dk1ObjectFramePieceGroup group;
} Dk1ObjectFramePiece;

typedef struct Dk1ObjectFrameLayout {
    uint16_t frame_id;
    uint32_t source_pc;
    uint8_t large_count;
    uint8_t small_count;
    uint8_t small_tile_offset;
    uint8_t auxiliary_count;
    uint8_t auxiliary_tile_offset;
    uint8_t attribute_5;
    uint8_t attribute_6;
    uint8_t attribute_7;
    size_t piece_count;
    Dk1ObjectFramePiece pieces[DK1_OBJECT_FRAME_MAX_PIECES];
} Dk1ObjectFrameLayout;

typedef struct Dk1ObjectFrameOamStyle {
    uint16_t base_tile_number;
    uint8_t palette;
    uint8_t priority;
    bool horizontal_flip;
    bool vertical_flip;
} Dk1ObjectFrameOamStyle;

bool dk1_object_frame_layout_read(const Dk1RomImage *rom, uint16_t frame_id,
                                  Dk1ObjectFrameLayout *layout);
bool dk1_object_frame_layout_emit_oam(const Dk1ObjectFrameLayout *layout,
                                      int16_t original_base_x,
                                      int16_t original_base_y,
                                      Dk1ObjectFrameOamStyle style,
                                      uint8_t obsel,
                                      size_t first_sprite,
                                      Dk1OamImage *oam,
                                      size_t *written);
uint64_t dk1_object_frame_layout_signature(const Dk1ObjectFrameLayout *layout);

#endif
