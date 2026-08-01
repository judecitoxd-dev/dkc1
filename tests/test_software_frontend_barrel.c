#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "dk1/scene_memory.h"
#include "dk1/software_frontend.h"

int main(void) {
    const char *path = getenv("DK1_TEST_ROM");
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom;
    Dk1SceneMemory scene;
    Dk1SoftwareFrontend frontend;
    Dk1Rgba8 pixels[96u * 64u];
    Dk1RgbaSurface surface = {pixels, 96u, 64u, 96u};

    if (path == NULL) path = "/mnt/data/Donkey Kong Country (USA) (Rev 2).sfc";
    assert(dk1_rom_load_file(path, &owned, &rom));
    assert(dk1_scene_memory_load(&rom, 0u, false, false, &scene));
    assert(dk1_software_frontend_init_with_rom(&rom, &scene, 96u, 64u,
                                               &frontend));
    assert(frontend.player_terrain_ready);
    assert(dk1_software_frontend_spawn_barrel(
        &frontend, DK1_OBJECT_TYPE_BARREL,
        (uint16_t)(frontend.player_preview.motion.world_x + 12u),
        (int16_t)(frontend.player_preview.motion.world_y - 8)));

    assert(dk1_software_frontend_step(&scene, DK1_HOST_BUTTON_Y, &frontend));
    assert(frontend.barrel.live.phase == DK1_BARREL_LIVE_HELD);
    assert(frontend.barrel.animation.frame == 0x1C18u);
    memset(pixels, 0, sizeof(pixels));
    assert(dk1_software_frontend_render(&rom, &scene, &frontend, surface));
    assert(frontend.barrel.visual_ready);
    assert(frontend.barrel.visual.pieces == 6u);
    assert(frontend.barrel.visual.dma_bytes == 576u);

    assert(dk1_software_frontend_step(&scene, 0u, &frontend));
    assert(dk1_software_frontend_step(&scene, DK1_HOST_BUTTON_Y, &frontend));
    assert(frontend.barrel.live.phase == DK1_BARREL_LIVE_THROWN);
    assert(frontend.barrel.animation.frame == 0x1BF8u);
    memset(pixels, 0, sizeof(pixels));
    assert(dk1_software_frontend_render(&rom, &scene, &frontend, surface));
    assert(frontend.barrel.visual.pieces == 6u);
    assert(frontend.barrel.visual.dma_bytes == 576u);

    dk1_rom_free(&owned);
    return 0;
}
