#include <assert.h>
#include <stdlib.h>
#include "dk1/cgram_image.h"
int main(){uint8_t*b=calloc(DK1_EXPECTED_ROM_SIZE,1);Dk1RomImage r;Dk1CgramImage c;size_t o;assert(b&&dk1_rom_image_init(&r,b,DK1_EXPECTED_ROM_SIZE));assert(dk1_hirom_offset(0xB9A000,DK1_EXPECTED_ROM_SIZE,&o));b[o]=0x1F;b[o+1]=0;b[o+2]=0;b[o+3]=0x7C;dk1_cgram_init(&c);assert(dk1_cgram_upload_b9(&r,0xA000,4,1,&c));assert(c.colors[4]==0x001F&&c.colors[5]==0x7C00);free(b);}
