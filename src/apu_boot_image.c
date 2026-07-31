#include "dk1/apu_boot_image.h"
#include <string.h>
bool dk1_apu_boot_image_read(const Dk1RomImage*r,Dk1ApuBootImage*i){size_t n;uint64_t h=14695981039346656037ull;if(!r||!i)return false;memset(i,0,sizeof(*i));if(!dk1_rom_copy(r,DK1_APU_BOOT_IMAGE_SOURCE,i->bytes,DK1_APU_BOOT_IMAGE_SIZE))return false;for(n=0;n<DK1_APU_BOOT_IMAGE_SIZE;n++){h^=i->bytes[n];h*=1099511628211ull;}i->signature=h;return true;}
