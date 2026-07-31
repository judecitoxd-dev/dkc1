#include "dk1/cgram_image.h"
#include <string.h>
void dk1_cgram_init(Dk1CgramImage *image) { if (image) memset(image,0,sizeof(*image)); }
bool dk1_cgram_upload_b9(const Dk1RomImage *rom, uint16_t source_address,
                         uint16_t cgram_start, uint16_t x_count,
                         Dk1CgramImage *image) {
    size_t byte_count=(size_t)x_count*8u, color_count=byte_count/2u, i;
    if (!rom || !image || cgram_start+color_count>DK1_CGRAM_COLOR_COUNT) return false;
    for(i=0;i<color_count;i++) {
        uint16_t color=0;
        if(!dk1_rom_read_u16(rom,0xB90000u|((source_address+(uint16_t)(i*2u))&0xFFFFu),&color)) return false;
        image->colors[cgram_start+i]=color;
    }
    return true;
}
void dk1_cgram_decode_rgba(const Dk1CgramImage *image, Dk1Rgba8 output[DK1_CGRAM_COLOR_COUNT]) {
    size_t i; if(!image||!output)return; for(i=0;i<DK1_CGRAM_COLOR_COUNT;i++) output[i]=dk1_snes_bgr555_to_rgba(image->colors[i],false);
}
