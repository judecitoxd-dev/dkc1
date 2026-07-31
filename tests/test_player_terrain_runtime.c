#include <assert.h>
#include <stdlib.h>
#include "dk1/level_terrain_config.h"
#include "dk1/player_terrain_runtime.h"
int main(void){const char*path=getenv("DK1_TEST_ROM");Dk1OwnedRom owned={0};Dk1RomImage rom;Dk1LevelTerrainConfig cfg;Dk1PlayerTerrainRuntime terrain;Dk1PlayerPreviewRuntime player;uint16_t camera_y=0;unsigned i;if(!path)path="/mnt/data/Donkey Kong Country (USA) (Rev 2).sfc";assert(dk1_rom_load_file(path,&owned,&rom));assert(dk1_level_terrain_config_read(&rom,0,&cfg));assert(dk1_player_terrain_init(&terrain,&rom,cfg,5120u));dk1_player_preview_init(&player,48u,192);assert(dk1_player_terrain_place(&terrain,&player,64u,&camera_y));assert(player.motion.world_y==352);assert(camera_y==128u);assert(terrain.support.floor_y==336);assert(!player.airborne);dk1_player_terrain_step(&terrain,&player,DK1_PLAYER_PREVIEW_JUMP,DK1_PLAYER_PREVIEW_JUMP);assert(player.airborne);for(i=0;i<200u&&player.airborne;i++)dk1_player_terrain_step(&terrain,&player,0u,0u);assert(!player.airborne);assert(player.motion.world_y>=352&&player.motion.world_y<=367);assert(player.floor_attributes==terrain.last_attributes);
assert(dk1_level_terrain_config_read(&rom,1,&cfg));
assert(dk1_player_terrain_init(&terrain,&rom,cfg,44544u));
dk1_player_preview_init(&player,34352u,192);camera_y=0;
assert(dk1_player_terrain_place(&terrain,&player,224u,&camera_y));
assert(player.motion.world_x==34352u);
assert(player.motion.world_y>=192&&player.motion.world_y<=207);
assert((int16_t)(uint16_t)(player.motion.world_x-34304u)==48);
dk1_rom_free(&owned);return 0;}
