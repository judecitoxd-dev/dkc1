#ifndef DK1_TERRAIN_CONTACTS_H
#define DK1_TERRAIN_CONTACTS_H

#include <stdbool.h>
#include <stdint.h>
#include "dk1/rom_terrain.h"

typedef struct Dk1TerrainContacts {
    Dk1TerrainSample left;
    Dk1TerrainSample center;
    Dk1TerrainSample right;
    bool any;
    bool all;
    int32_t nearest_floor_y;
    uint16_t combined_attributes;
} Dk1TerrainContacts;

bool dk1_terrain_contacts(const Dk1TerrainLayout *layout,
                          int32_t center_x, int32_t probe_y,
                          int16_t half_width, size_t max_cells,
                          Dk1TerrainContacts *contacts);
bool dk1_rom_terrain_contacts(const Dk1RomTerrainView *view,
                              int32_t center_x, int32_t probe_y,
                              int16_t half_width, size_t max_cells,
                              Dk1TerrainContacts *contacts);

#endif
