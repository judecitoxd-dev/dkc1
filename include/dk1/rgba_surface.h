#ifndef DK1_RGBA_SURFACE_H
#define DK1_RGBA_SURFACE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/snes_graphics.h"
#include "dk1/tilemap_renderer.h"

typedef struct Dk1RgbaSurface {
    Dk1Rgba8 *pixels;
    size_t width;
    size_t height;
    size_t stride;
} Dk1RgbaSurface;

bool dk1_surface_indexed_to_rgba(Dk1RgbaSurface destination, Dk1IndexedSurface source, const Dk1Rgba8 *palette, size_t palette_size);
void dk1_surface_fill_rect(Dk1RgbaSurface surface, int32_t x, int32_t y, uint32_t width, uint32_t height, Dk1Rgba8 color);
bool dk1_surface_write_ppm(const char *path, Dk1RgbaSurface surface);

#endif
