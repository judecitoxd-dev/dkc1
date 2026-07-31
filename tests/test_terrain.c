#include <assert.h>
#include "dk1/terrain.h"

int main(void) {
    uint16_t map[16] = {0};
    uint16_t halves[4] = {0};
    uint16_t flags[64] = {0};
    Dk1TerrainLayout layout = {map, 1u, halves, 2u, flags};
    Dk1TerrainSample sample;
    int16_t height = 0;

    assert(dk1_terrain_shape_height(1u, 4u, &height) && height == 31);
    assert(dk1_terrain_shape_height(57u, 3u, &height) && height == 12);
    assert(dk1_terrain_shape_height(59u, 12u, &height) && height == 15);
    assert(dk1_terrain_shape_height(60u, 2u, &height) && height == 15);
    assert(!dk1_terrain_shape_height(63u, 0u, &height));

    map[15] = 1u;
    halves[2] = 57u;
    halves[3] = 1u;
    flags[57] = 0x0005u;
    assert(dk1_terrain_sample_cell(&layout, 3, 0, &sample));
    assert(sample.floor_y == 12);
    assert(sample.shape_id == 57u);
    assert(sample.attributes == 5u);
    assert(dk1_terrain_sample_cell(&layout, 20, 0, &sample));
    assert(sample.floor_y == 31);

    map[15] = (uint16_t)(0x4000u | 1u);
    assert(dk1_terrain_sample_cell(&layout, 3, 0, &sample));
    assert(sample.shape_id == 1u);

    map[15] = 1u;
    halves[2] = (uint16_t)(0x0080u | 57u);
    assert(dk1_terrain_sample_cell(&layout, 3, 0, &sample));
    assert(sample.local_x == 12u);
    assert(sample.floor_y == 31);
    return 0;
}
