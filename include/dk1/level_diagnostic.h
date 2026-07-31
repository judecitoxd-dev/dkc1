#ifndef DK1_LEVEL_DIAGNOSTIC_H
#define DK1_LEVEL_DIAGNOSTIC_H

#include <stdbool.h>
#include <stddef.h>
#include "dk1/level_map.h"
#include "dk1/rgba_surface.h"
#include "dk1/rom_terrain.h"

bool dk1_level_diagnostic_render(const Dk1RomImage *rom,
                                 Dk1LevelTerrainConfig config,
                                 size_t first_column, size_t column_count,
                                 Dk1RgbaSurface surface);

#endif
