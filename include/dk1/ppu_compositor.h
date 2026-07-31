#ifndef DK1_PPU_COMPOSITOR_H
#define DK1_PPU_COMPOSITOR_H
#include <stdbool.h>
#include <stdint.h>
#include "dk1/ppu_bg_renderer.h"
#include "dk1/scene_memory.h"
bool dk1_ppu_compose_mode1(const Dk1SceneMemory *scene,uint16_t bg2x,uint16_t bg2y,uint16_t bg3x,uint16_t bg3y,Dk1RgbaSurface surface);
#endif
