#include "dk1/terrain_contacts.h"
#include <limits.h>

static bool one_rom(const Dk1RomTerrainView *view, int32_t x, int32_t y,
                    size_t max_cells, Dk1TerrainSample *sample) {
    return dk1_rom_terrain_find_floor(view, x, y, max_cells, sample) && sample->hit;
}

static bool one_layout(const Dk1TerrainLayout *layout, int32_t x, int32_t y,
                       size_t max_cells, Dk1TerrainSample *sample) {
    return dk1_terrain_find_floor(layout, x, y, max_cells, sample) && sample->hit;
}

static void finish_contacts(bool l, bool c, bool r, Dk1TerrainContacts *contacts) {
    int32_t nearest = INT32_MAX;
    contacts->any = l || c || r;
    contacts->all = l && c && r;
    if (l && contacts->left.floor_y < nearest) nearest = contacts->left.floor_y;
    if (c && contacts->center.floor_y < nearest) nearest = contacts->center.floor_y;
    if (r && contacts->right.floor_y < nearest) nearest = contacts->right.floor_y;
    contacts->nearest_floor_y = contacts->any ? nearest : 0;
    if (l) contacts->combined_attributes |= contacts->left.attributes;
    if (c) contacts->combined_attributes |= contacts->center.attributes;
    if (r) contacts->combined_attributes |= contacts->right.attributes;
}

bool dk1_terrain_contacts(const Dk1TerrainLayout *layout,
                          int32_t center_x, int32_t probe_y,
                          int16_t half_width, size_t max_cells,
                          Dk1TerrainContacts *contacts) {
    bool l, c, r;
    if (layout == NULL || contacts == NULL || half_width < 0) return false;
    *contacts = (Dk1TerrainContacts){0};
    l = one_layout(layout, center_x - half_width, probe_y, max_cells, &contacts->left);
    c = one_layout(layout, center_x, probe_y, max_cells, &contacts->center);
    r = one_layout(layout, center_x + half_width, probe_y, max_cells, &contacts->right);
    finish_contacts(l, c, r, contacts);
    return contacts->any;
}

bool dk1_rom_terrain_contacts(const Dk1RomTerrainView *view,
                              int32_t center_x, int32_t probe_y,
                              int16_t half_width, size_t max_cells,
                              Dk1TerrainContacts *contacts) {
    bool l, c, r;
    if (view == NULL || contacts == NULL || half_width < 0) return false;
    *contacts = (Dk1TerrainContacts){0};
    l = one_rom(view, center_x - half_width, probe_y, max_cells, &contacts->left);
    c = one_rom(view, center_x, probe_y, max_cells, &contacts->center);
    r = one_rom(view, center_x + half_width, probe_y, max_cells, &contacts->right);
    finish_contacts(l, c, r, contacts);
    return contacts->any;
}
