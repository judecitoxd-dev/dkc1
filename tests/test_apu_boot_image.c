#include <assert.h>
#include <string.h>
#include "dk1/apu_boot_image.h"
int main(void){static uint8_t rom[0x400000];Dk1RomImage r;Dk1ApuBootImage i;size_t off=((0x8Au&0x3Fu)<<16)|0xA342u;for(size_t n=0;n<40;n++)rom[off+n]=(uint8_t)n;assert(dk1_rom_image_init(&r,rom,sizeof(rom)));assert(dk1_apu_boot_image_read(&r,&i));assert(i.bytes[0]==0&&i.bytes[39]==39&&i.signature!=0);assert(!dk1_apu_boot_image_read(0,&i));return 0;}
