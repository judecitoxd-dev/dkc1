#include <assert.h>
#include "dk1/terrain_contacts.h"
int main(void) {
    uint16_t map[16]={0}; uint16_t halves[4]={0}; uint16_t flags[64]={0};
    Dk1TerrainLayout layout={map,1u,halves,2u,flags}; Dk1TerrainContacts c;
    map[15]=1u; halves[2]=1u; halves[3]=1u; flags[1]=0x0004u;
    assert(dk1_terrain_contacts(&layout,16,0,8,0,&c));
    assert(c.any && c.all && c.nearest_floor_y==31);
    assert((c.combined_attributes & 4u)!=0u);
    map[15]=0u; assert(!dk1_terrain_contacts(&layout,16,0,8,0,&c));
    return 0;
}
