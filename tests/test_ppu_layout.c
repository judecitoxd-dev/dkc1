#include <assert.h>
#include <string.h>
#include "dk1/ppu_layout.h"
static void w(Dk1PpuPreset*p,int a,int v){p->written[a-0x2100]=1;p->values[a-0x2100]=(uint8_t)v;}
int main(){Dk1PpuPreset p;Dk1PpuBgLayout l;memset(&p,0,sizeof(p));w(&p,0x2105,1);w(&p,0x2107,0x6D);w(&p,0x2108,0x75);w(&p,0x2109,0x24);w(&p,0x210A,0);w(&p,0x210B,0x33);w(&p,0x210C,2);w(&p,0x212C,0x17);assert(dk1_ppu_bg_layout(&p,1,&l)&&l.bits_per_pixel==4&&l.screen_base_word==0x6C00&&l.character_base_word==0x3000&&l.tilemap_width==64);assert(dk1_ppu_bg_layout(&p,3,&l)&&l.bits_per_pixel==2&&l.main_enabled);}
