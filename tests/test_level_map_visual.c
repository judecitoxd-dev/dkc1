#include <assert.h>
#include <stdlib.h>
#include "dk1/level_map.h"
#include "dk1/level_metatile.h"
static void put16(uint8_t *bytes,uint32_t address,uint16_t value){size_t offset;assert(dk1_hirom_offset(address,DK1_EXPECTED_ROM_SIZE,&offset));bytes[offset]=(uint8_t)value;bytes[offset+1u]=(uint8_t)(value>>8u);}
int main(void){uint8_t*bytes=(uint8_t*)calloc(DK1_EXPECTED_ROM_SIZE,1u);Dk1RomImage rom;Dk1LevelTerrainConfig config={0};uint16_t value,metatile[DK1_METATILE_WORD_COUNT];size_t i;assert(bytes!=NULL&&dk1_rom_image_init(&rom,bytes,DK1_EXPECTED_ROM_SIZE));config.map_snes=0xC01000u;config.visual_bank=0x80u;config.visual_block_base=0x7E28u;put16(bytes,config.map_snes+2u,0x1111u);put16(bytes,config.map_snes+128u,0x2222u);assert(dk1_level_map_cell(&rom,&config,1u,0u,&value)&&value==0x1111u);assert(dk1_level_map_cell(&rom,&config,0u,1u,&value)&&value==0x2222u);assert(dk1_level_metatile_expand(&rom,&config,0u,metatile));for(i=0u;i<DK1_METATILE_WORD_COUNT;++i)assert(metatile[i]==0u);free(bytes);return 0;}
