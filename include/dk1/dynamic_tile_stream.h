#ifndef DK1_DYNAMIC_TILE_STREAM_H
#define DK1_DYNAMIC_TILE_STREAM_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/dynamic_stream.h"
#include "dk1/level_map.h"
#include "dk1/ppu_layout.h"
#include "dk1/vram_image.h"
typedef struct Dk1DynamicTileStreamStats { size_t map_columns; size_t tile_columns; size_t tile_words; } Dk1DynamicTileStreamStats;
bool dk1_dynamic_tile_stream_apply(const Dk1RomImage *rom,
                                   const Dk1LevelTerrainConfig *config,
                                   const Dk1PpuBgLayout *layout,
                                   const Dk1DynamicStreamUpdate *update,
                                   Dk1VramImage *vram,
                                   Dk1DynamicTileStreamStats *stats);
#endif
