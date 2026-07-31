#include "dk1/bg_renderer.h"
bool dk1_bg_render_4bpp(const Dk1VramImage *vram, const Dk1CgramImage *cgram,
                        uint16_t character_base_word,
                        const uint16_t *tilemap, size_t tile_width, size_t tile_height,
                        Dk1RgbaSurface surface) {
    Dk1Rgba8 rgba[DK1_CGRAM_COLOR_COUNT]; size_t ty,tx;
    if(!vram||!cgram||!tilemap||!surface.pixels||surface.width<tile_width*8u||surface.height<tile_height*8u||surface.stride<surface.width)return false;
    dk1_cgram_decode_rgba(cgram,rgba);
    for(ty=0;ty<tile_height;ty++) for(tx=0;tx<tile_width;tx++) {
        uint16_t w=tilemap[ty*tile_width+tx]; uint16_t id=w&0x03FFu; uint8_t pal=(uint8_t)((w>>10u)&7u); bool hf=(w&0x4000u)!=0,vf=(w&0x8000u)!=0;
        size_t base=((size_t)character_base_word*2u+(size_t)id*32u)&0xFFFFu; uint8_t pix[64]; size_t y,x;
        uint8_t raw[32]; for(x=0;x<32;x++) raw[x]=vram->bytes[(base+x)&0xFFFFu];
        if(!dk1_snes_decode_4bpp_tile(raw,pix))return false;
        for(y=0;y<8;y++)for(x=0;x<8;x++) { size_t sx=hf?7u-x:x, sy=vf?7u-y:y; uint8_t p=pix[sy*8u+sx]; Dk1Rgba8 color=rgba[(size_t)pal*16u+p];
            surface.pixels[(ty*8u+y)*surface.stride+(tx*8u+x)]=color;
        }
    }
    return true;
}
