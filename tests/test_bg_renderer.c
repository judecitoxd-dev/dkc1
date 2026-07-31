#include <assert.h>
#include "dk1/bg_renderer.h"
int main(){Dk1VramImage v;Dk1CgramImage c;Dk1Rgba8 px[64];Dk1RgbaSurface s={px,8,8,8};uint16_t map=0;dk1_vram_image_init(&v);dk1_cgram_init(&c);v.bytes[0]=0x80;c.colors[1]=0x001F;assert(dk1_bg_render_4bpp(&v,&c,0,&map,1,1,s));assert(px[0].r==255);}
