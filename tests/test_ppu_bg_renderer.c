#include <assert.h>
#include "dk1/ppu_bg_renderer.h"
int main(){Dk1VramImage v;Dk1CgramImage c;Dk1PpuBgLayout l={1,1,4,0x100,0,32,32,false,true};Dk1Rgba8 px[64];Dk1RgbaSurface s={px,8,8,8};dk1_vram_image_init(&v);dk1_cgram_init(&c);v.bytes[0x200]=0;v.bytes[0x201]=0;v.bytes[0]=0x80;c.colors[1]=0x03E0;assert(dk1_ppu_render_vram_bg(&v,&c,&l,0,0,s,false));assert(px[0].g==255);}
