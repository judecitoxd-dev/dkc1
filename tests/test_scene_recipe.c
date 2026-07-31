#include <assert.h>
#include <stdlib.h>
#include "dk1/scene_recipe.h"
static void p16(uint8_t*b,uint32_t a,uint16_t v){size_t o;assert(dk1_hirom_offset(a,DK1_EXPECTED_ROM_SIZE,&o));b[o]=(uint8_t)v;b[o+1]=(uint8_t)(v>>8);}
int main(){uint8_t*b=calloc(DK1_EXPECTED_ROM_SIZE,1);Dk1RomImage r;Dk1SceneRecipe x;assert(b&&dk1_rom_image_init(&r,b,DK1_EXPECTED_ROM_SIZE));p16(b,0xB9801E,0x8590);assert(dk1_scene_recipe_count()==31);assert(dk1_scene_recipe_for_level(&r,0,&x)&&x.ppu_preset==1&&x.terrain_profile==0&&x.primary_vram_package==1);assert(dk1_scene_recipe_for_callback(0x87B7,&x)&&x.patch_special_palette);assert(!dk1_scene_recipe_for_callback(0,&x));free(b);}
