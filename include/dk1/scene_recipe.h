#ifndef DK1_SCENE_RECIPE_H
#define DK1_SCENE_RECIPE_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/rom_image.h"
#define DK1_SCENE_LEVEL_COUNT 230u
#define DK1_SCENE_RECIPE_COUNT 31u
#define DK1_SCENE_PALETTE_UPLOAD_MAX 2u
typedef enum Dk1PaletteUploadMode { DK1_PALETTE_UPLOAD_NORMAL=0, DK1_PALETTE_UPLOAD_INVERT_BYTES=1 } Dk1PaletteUploadMode;
typedef struct Dk1ScenePaletteUpload { uint16_t source_b9; uint8_t cgram_start; uint8_t x_count; Dk1PaletteUploadMode mode; } Dk1ScenePaletteUpload;
typedef struct Dk1SceneRecipe { uint16_t init_callback_b9; uint8_t ppu_preset; uint8_t terrain_profile; uint8_t primary_vram_package; uint8_t palette_upload_count; Dk1ScenePaletteUpload palette_uploads[DK1_SCENE_PALETTE_UPLOAD_MAX]; bool patch_special_palette; } Dk1SceneRecipe;
size_t dk1_scene_recipe_count(void);
bool dk1_scene_recipe_for_callback(uint16_t callback_b9,Dk1SceneRecipe *recipe);
bool dk1_scene_recipe_for_level(const Dk1RomImage *rom,uint16_t level_id,Dk1SceneRecipe *recipe);
#endif
