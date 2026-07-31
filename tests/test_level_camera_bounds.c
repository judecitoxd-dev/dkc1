#include <assert.h>
#include <stdlib.h>
#include "dk1/level_camera_bounds.h"
static void p16(uint8_t*b,uint32_t a,uint16_t v){size_t o;assert(dk1_hirom_offset(a,DK1_EXPECTED_ROM_SIZE,&o));b[o]=(uint8_t)v;b[o+1]=(uint8_t)(v>>8);}
int main(){uint8_t*b=calloc(DK1_EXPECTED_ROM_SIZE,1);Dk1RomImage r;Dk1LevelCameraBounds x;assert(b&&dk1_rom_image_init(&r,b,DK1_EXPECTED_ROM_SIZE));p16(b,0xBC8000,0x8104);p16(b,0xBC8100,0x20);p16(b,0xBC8102,0x500);assert(dk1_level_camera_bounds_read(&r,0,&x)&&x.minimum_x==0x20&&x.maximum_x==0x500);free(b);}
