#ifndef DK1_PPU_LAYOUT_H
#define DK1_PPU_LAYOUT_H
#include <stdbool.h>
#include <stdint.h>
#include "dk1/ppu_preset.h"
typedef struct Dk1PpuBgLayout { uint8_t bg; uint8_t mode; uint8_t bits_per_pixel; uint16_t screen_base_word; uint16_t character_base_word; uint8_t tilemap_width; uint8_t tilemap_height; bool large_tiles; bool main_enabled; } Dk1PpuBgLayout;
bool dk1_ppu_bg_layout(const Dk1PpuPreset *preset,uint8_t bg,Dk1PpuBgLayout *layout);
#endif
