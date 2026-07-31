#include <assert.h>
#include <string.h>
#include "dk1/snes_graphics.h"
int main(){uint8_t a[64]={0},p[64];a[0]=0x80;a[1]=0x80;assert(dk1_snes_decode_2bpp_tile(a,p)&&p[0]==3);memset(a,0,sizeof(a));a[0]=0x80;a[17]=0x80;a[32]=0x80;a[49]=0x80;assert(dk1_snes_decode_8bpp_tile(a,p)&&p[0]==0x99);}
