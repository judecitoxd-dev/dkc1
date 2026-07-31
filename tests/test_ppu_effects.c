#include <assert.h>
#include "dk1/ppu_effects.h"
int main(void) {
    Dk1PpuWindowState w = {10,20,30,40,0x02,0,0,0,0};
    Dk1PpuCandidate c[3] = {
        {DK1_PPU_LAYER_BG3,1,1,true},
        {DK1_PPU_LAYER_BG1,1,2,true},
        {DK1_PPU_LAYER_OBJ,3,3,true}
    }, out;
    Dk1Rgba8 dark;
    assert(dk1_ppu_color_math_bgr555(0x0010u,0x0008u,false,false)==0x0018u);
    assert(dk1_ppu_color_math_bgr555(0x0010u,0x0008u,true,false)==0x0008u);
    assert(dk1_ppu_color_math_bgr555(0x0010u,0x0008u,false,true)==0x000Cu);
    dark=dk1_ppu_apply_brightness(0x001Fu,8u);
    assert(dark.r>120u && dark.r<150u && dark.g==0u && dark.b==0u);
    assert(dk1_ppu_window_result(&w,DK1_PPU_LAYER_BG1,15u));
    assert(!dk1_ppu_window_result(&w,DK1_PPU_LAYER_BG1,25u));
    assert(dk1_ppu_select_mode1(c,3u,false,&out));
    assert(out.layer==DK1_PPU_LAYER_OBJ);
    assert(dk1_ppu_select_mode1(c,3u,true,&out));
    assert(out.layer==DK1_PPU_LAYER_BG3);
    return 0;
}
