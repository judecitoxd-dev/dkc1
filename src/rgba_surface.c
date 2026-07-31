#include "dk1/rgba_surface.h"

#include <stdio.h>

bool dk1_surface_indexed_to_rgba(Dk1RgbaSurface destination, Dk1IndexedSurface source, const Dk1Rgba8 *palette, size_t palette_size) {
    size_t y = 0u;
    if (destination.pixels == NULL || source.pixels == NULL || palette == NULL || palette_size == 0u || destination.width != source.width || destination.height != source.height || destination.stride < destination.width || source.stride < source.width) return false;
    for (y = 0u; y < source.height; ++y) {
        size_t x = 0u;
        for (x = 0u; x < source.width; ++x) {
            const uint8_t index = source.pixels[y * source.stride + x];
            destination.pixels[y * destination.stride + x] = palette[(size_t)index % palette_size];
        }
    }
    return true;
}

void dk1_surface_fill_rect(Dk1RgbaSurface surface, int32_t x, int32_t y, uint32_t width, uint32_t height, Dk1Rgba8 color) {
    uint32_t py = 0u;
    if (surface.pixels == NULL || surface.stride < surface.width) return;
    for (py = 0u; py < height; ++py) {
        const int64_t dy = (int64_t)y + py;
        uint32_t px = 0u;
        if (dy < 0 || dy >= (int64_t)surface.height) continue;
        for (px = 0u; px < width; ++px) {
            const int64_t dx = (int64_t)x + px;
            if (dx < 0 || dx >= (int64_t)surface.width) continue;
            surface.pixels[(size_t)dy * surface.stride + (size_t)dx] = color;
        }
    }
}

bool dk1_surface_write_ppm(const char *path, Dk1RgbaSurface surface) {
    FILE *file = NULL;
    size_t y = 0u;
    if (path == NULL || surface.pixels == NULL || surface.width == 0u || surface.height == 0u || surface.stride < surface.width) return false;
    file = fopen(path, "wb");
    if (file == NULL) return false;
    if (fprintf(file, "P6\n%zu %zu\n255\n", surface.width, surface.height) < 0) { fclose(file); return false; }
    for (y = 0u; y < surface.height; ++y) {
        size_t x = 0u;
        for (x = 0u; x < surface.width; ++x) {
            const Dk1Rgba8 color = surface.pixels[y * surface.stride + x];
            const uint8_t rgb[3] = {color.r, color.g, color.b};
            if (fwrite(rgb, 1u, sizeof(rgb), file) != sizeof(rgb)) { fclose(file); return false; }
        }
    }
    return fclose(file) == 0;
}
