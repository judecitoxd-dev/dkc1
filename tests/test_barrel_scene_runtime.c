#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "dk1/barrel_scene_runtime.h"
#include "dk1/host_input.h"
#include "dk1/level_terrain_config.h"
#include "dk1/rom_image.h"

int main(void) {
    const char *path = getenv("DK1_TEST_ROM");
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom;
    Dk1LevelTerrainConfig config;
    Dk1PlayerTerrainRuntime terrain;
    Dk1PlayerPreviewRuntime player;
    Dk1BarrelSceneRuntime barrel;
    Dk1BarrelLiveResult result;
    Dk1VramImage base = {{0}};
    uint16_t camera_y = 0u;

    if (path == NULL) path = "/mnt/data/Donkey Kong Country (USA) (Rev 2).sfc";
    assert(dk1_rom_load_file(path, &owned, &rom));
    assert(dk1_level_terrain_config_read(&rom, 0u, &config));
    assert(dk1_player_terrain_init(&terrain, &rom, config, 5120u));
    dk1_player_preview_init(&player, 48u, 192);
    assert(dk1_player_terrain_place(&terrain, &player, 224u, &camera_y));

    assert(dk1_barrel_scene_spawn(&barrel, &rom, 2u,
                                  DK1_OBJECT_TYPE_BARREL,
                                  (uint16_t)(player.motion.world_x + 12u),
                                  (int16_t)(player.motion.world_y - 8)));
    assert(barrel.selected_animation == DK1_ANIMATION_BARREL_IDLE);
    assert(barrel.animation.frame == 0x1BD4u);
    assert(dk1_barrel_scene_build_visual(&barrel, &rom, &base,
                                         0u, camera_y, 512u, 0u));
    assert(barrel.visual.pieces == 7u);
    assert(barrel.visual.dma_bytes == 608u);

    assert(dk1_barrel_scene_step(&barrel, &rom, &terrain, &player,
                                 DK1_HOST_BUTTON_Y, &result));
    assert(result.picked_up);
    assert(barrel.live.phase == DK1_BARREL_LIVE_HELD);
    assert(barrel.selected_animation == 0x00D8u);
    assert(barrel.animation.frame == 0x1C18u);
    assert(dk1_barrel_scene_build_visual(&barrel, &rom, &base,
                                         0u, camera_y, 512u, 0u));
    assert(barrel.visual.pieces == 6u);
    assert(barrel.visual.dma_bytes == 576u);

    assert(dk1_barrel_scene_step(&barrel, &rom, &terrain, &player, 0u, &result));
    assert(dk1_barrel_scene_step(&barrel, &rom, &terrain, &player,
                                 DK1_HOST_BUTTON_Y, &result));
    assert(result.thrown);
    assert(barrel.live.phase == DK1_BARREL_LIVE_THROWN);
    assert(barrel.selected_animation == 0x00DEu);
    assert(barrel.animation.frame == 0x1BF8u);
    assert(dk1_barrel_scene_build_visual(&barrel, &rom, &base,
                                         0u, camera_y, 512u, 0u));
    assert(barrel.visual.pieces == 6u);
    assert(barrel.visual.dma_bytes == 576u);

    dk1_rom_free(&owned);
    return 0;
}
