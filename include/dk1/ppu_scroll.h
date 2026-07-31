#ifndef DK1_PPU_SCROLL_H
#define DK1_PPU_SCROLL_H

#include <stdint.h>

typedef struct Dk1PpuScrollState {
    uint16_t bg1_x;
    uint16_t bg1_y;
    uint16_t bg2_x;
    uint16_t bg2_y;
    uint16_t bg3_x;
    uint16_t bg3_y;
} Dk1PpuScrollState;

/*
 * Scroll transform written by $80:8973-$80:89B3:
 * BG1 follows the camera, BG2 uses half-speed parallax, BG3 vertical is zero.
 */
Dk1PpuScrollState dk1_ppu_scroll_half_parallax(
    uint16_t camera_x,
    uint16_t camera_y
);

#endif
