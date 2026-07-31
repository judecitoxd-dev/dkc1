#ifndef DK1_BG_RENDERER_H
#define DK1_BG_RENDERER_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/vram_image.h"
#include "dk1/cgram_image.h"
#include "dk1/rgba_surface.h"
bool dk1_bg_render_4bpp(const Dk1VramImage *vram, const Dk1CgramImage *cgram,
                        uint16_t character_base_word,
                        const uint16_t *tilemap, size_t tile_width, size_t tile_height,
                        Dk1RgbaSurface surface);
#endif
