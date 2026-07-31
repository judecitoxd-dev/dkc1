#ifndef DK1_TILEMAP_RENDERER_H
#define DK1_TILEMAP_RENDERER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DK1_TILE_SIZE 8u

typedef struct Dk1TileAtlas {
    const uint8_t *pixels;
    size_t tile_count;
} Dk1TileAtlas;

typedef struct Dk1Tilemap {
    const uint16_t *tiles;
    size_t width_tiles;
    size_t height_tiles;
} Dk1Tilemap;

typedef struct Dk1IndexedSurface {
    uint8_t *pixels;
    size_t width;
    size_t height;
    size_t stride;
} Dk1IndexedSurface;

/* Host renderer foundation. No original game graphics are embedded. */
bool dk1_tilemap_draw_wrapped(
    Dk1IndexedSurface surface,
    Dk1Tilemap map,
    Dk1TileAtlas atlas,
    uint16_t scroll_x,
    uint16_t scroll_y
);

#endif
