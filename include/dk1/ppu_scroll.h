#ifndef DK1_PPU_SCROLL_H
#define DK1_PPU_SCROLL_H

#include <stdint.h>

enum Dk1PpuScrollWriteMask {
    DK1_SCROLL_WRITE_BG1_X = 1u << 0,
    DK1_SCROLL_WRITE_BG1_Y = 1u << 1,
    DK1_SCROLL_WRITE_BG2_X = 1u << 2,
    DK1_SCROLL_WRITE_BG2_Y = 1u << 3,
    DK1_SCROLL_WRITE_BG3_X = 1u << 4,
    DK1_SCROLL_WRITE_BG3_Y = 1u << 5,
    DK1_SCROLL_WRITE_ALL = 0x3Fu,
};

typedef struct Dk1PpuScrollState {
    uint16_t bg1_x;
    uint16_t bg1_y;
    uint16_t bg2_x;
    uint16_t bg2_y;
    uint16_t bg3_x;
    uint16_t bg3_y;
    uint8_t write_mask;
} Dk1PpuScrollState;

/* $80:8973-$80:89B3. */
Dk1PpuScrollState dk1_ppu_scroll_half_parallax(
    uint16_t camera_x,
    uint16_t camera_y
);

/* $80:8CA0-$80:8CEA: BG2 vertical uses quarter speed. */
Dk1PpuScrollState dk1_ppu_scroll_quarter_vertical(
    uint16_t camera_x,
    uint16_t camera_y,
    uint16_t bg3_y
);

/* $80:8DF0-$80:8E42: BG3 horizontal uses 1.5x camera speed. */
Dk1PpuScrollState dk1_ppu_scroll_bg3_one_and_half(
    uint16_t camera_x,
    uint16_t camera_y
);

#endif
