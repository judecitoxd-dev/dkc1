#ifndef DK1_PPU_EFFECTS_H
#define DK1_PPU_EFFECTS_H

#include <stdbool.h>
#include <stdint.h>
#include "dk1/snes_graphics.h"

typedef enum Dk1PpuLayer {
    DK1_PPU_LAYER_BG1 = 0,
    DK1_PPU_LAYER_BG2 = 1,
    DK1_PPU_LAYER_BG3 = 2,
    DK1_PPU_LAYER_BG4 = 3,
    DK1_PPU_LAYER_OBJ = 4,
    DK1_PPU_LAYER_BACKDROP = 5
} Dk1PpuLayer;

typedef struct Dk1PpuWindowState {
    uint8_t wh0, wh1, wh2, wh3;
    uint8_t w12sel, w34sel, wobjsel;
    uint8_t wbglog, wobjlog;
} Dk1PpuWindowState;

typedef struct Dk1PpuCandidate {
    Dk1PpuLayer layer;
    uint8_t priority;
    uint16_t color_bgr555;
    bool opaque;
} Dk1PpuCandidate;

uint16_t dk1_ppu_color_math_bgr555(uint16_t main_color, uint16_t sub_color,
                                    bool subtract, bool half);
Dk1Rgba8 dk1_ppu_apply_brightness(uint16_t color_bgr555, uint8_t brightness);
bool dk1_ppu_window_result(const Dk1PpuWindowState *state, Dk1PpuLayer layer,
                           uint8_t x);
int dk1_ppu_mode1_rank(Dk1PpuLayer layer, uint8_t priority,
                       bool bg3_high_priority);
bool dk1_ppu_select_mode1(const Dk1PpuCandidate *candidates, uint8_t count,
                          bool bg3_high_priority, Dk1PpuCandidate *selected);

#endif
