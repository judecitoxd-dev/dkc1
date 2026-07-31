#ifndef DK1_PPU_BG_RENDERER_H
#define DK1_PPU_BG_RENDERER_H
#include <stdbool.h>
#include <stdint.h>
#include "dk1/ppu_layout.h"
#include "dk1/vram_image.h"
#include "dk1/cgram_image.h"
#include "dk1/rgba_surface.h"
bool dk1_ppu_render_vram_bg(const Dk1VramImage *vram,const Dk1CgramImage *cgram,const Dk1PpuBgLayout *layout,uint16_t scroll_x,uint16_t scroll_y,Dk1RgbaSurface surface,bool transparent_zero);
#endif
