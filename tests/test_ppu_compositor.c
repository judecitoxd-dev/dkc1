#include <assert.h>
#include <string.h>
#include "dk1/ppu_compositor.h"
static void w(Dk1PpuPreset*p,int a,int v){p->written[a-0x2100]=1;p->values[a-0x2100]=(uint8_t)v;}
int main(){Dk1SceneMemory s;Dk1Rgba8 px[64];Dk1RgbaSurface o={px,8,8,8};memset(&s,0,sizeof(s));w(&s.ppu,0x2105,1);w(&s.ppu,0x2107,0);w(&s.ppu,0x2108,4);w(&s.ppu,0x2109,8);w(&s.ppu,0x210A,0);w(&s.ppu,0x210B,0);w(&s.ppu,0x210C,0);w(&s.ppu,0x212C,0x06);s.vram.bytes[0]=0x80;s.cgram.colors[1]=0x7C00;assert(dk1_ppu_compose_mode1(&s,0,0,0,0,o));assert(px[0].b==255);}
