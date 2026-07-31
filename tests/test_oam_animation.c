#include <assert.h>
#include "dk1/oam_animation.h"
int main(void) {
    Dk1OamAnimation animation;
    Dk1OamImage oam;
    Dk1OamSprite base = {10,20,0,0,3,false,false,false,8,8}, decoded;
    dk1_oam_animation_init(&animation,true);
    assert(dk1_oam_animation_add(&animation,(Dk1OamAnimationFrame){3,2,1,-1,false,false}));
    assert(dk1_oam_animation_add(&animation,(Dk1OamAnimationFrame){4,1,0,0,true,false}));
    assert(dk1_oam_animation_step(&animation));
    assert(animation.frame_index==0u);
    assert(dk1_oam_animation_step(&animation));
    assert(animation.frame_index==1u);
    dk1_oam_init(&oam);
    assert(dk1_oam_animation_apply(&animation,&oam,0u,0u,base));
    assert(dk1_oam_decode_sprite(&oam,0u,0u,&decoded));
    assert(decoded.tile_number==4u && decoded.horizontal_flip);
    assert(dk1_oam_animation_step(&animation));
    assert(animation.frame_index==0u);
    return 0;
}
