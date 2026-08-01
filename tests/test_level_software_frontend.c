#include <assert.h>
#include <stdlib.h>
#include "dk1/level_software_frontend.h"
#include "dk1/scene_memory.h"

int main(void) {
    const char *path = getenv("DK1_TEST_ROM");
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom;
    Dk1SceneMemory scene;
    Dk1SoftwareFrontend frontend;
    Dk1LevelSoftwareFrontendStats stats;
    if (path == NULL)
        path = "/mnt/data/Donkey Kong Country (USA) (Rev 2).sfc";
    assert(dk1_rom_load_file(path, &owned, &rom));
    assert(dk1_scene_memory_load(&rom, 0x0016u, false, false, &scene));
    assert(dk1_level_software_frontend_init(&rom, &scene, 0x0016u,
                                            96u, 64u,
                                            &frontend, &stats));
    assert(stats.entrance_id == 0x0016u);
    assert(stats.object_list_imported);
    assert(stats.primary_import.list.terminated);
    assert(stats.primary_import.list.record_count == 66u);
    assert(stats.primary_import.imported_count <=
           DK1_PRIMARY_OBJECT_SLOT_COUNT);
    assert(stats.primary_import.callback_count ==
           stats.primary_import.imported_count);
    assert(stats.primary_import.scheduler_verified);
    assert(stats.barrel_found && stats.barrel_spawned);
    assert(stats.barrel_spawn.record_index == 35u);
    assert(stats.barrel_spawn.record.source_pc == 0xBD96F4u);
    assert(stats.barrel_spawn.record.world_x == 0x0986u);
    assert((uint16_t)stats.barrel_spawn.record.world_y == 0x005Fu);
    assert(stats.barrel_spawn.record.definition_address == 0x92A9u);
    assert(stats.barrel_spawn.type_id == DK1_OBJECT_TYPE_BARREL);
    assert(stats.barrel_spawn.callback_pc == DK1_COMMON_BARREL_CALLBACK_PC);
    assert(stats.barrel_spawn.scheduler_verified);
    assert(frontend.barrel_ready);
    assert(frontend.barrel.live.object.type_id == DK1_OBJECT_TYPE_BARREL);
    assert(frontend.barrel.live.motion.world_x == 0x0986u);
    assert((uint16_t)frontend.barrel.live.motion.world_y == 0x005Fu);
    assert(frontend.barrel.animation.frame == 0x1BD4u);
    assert(frontend.barrel.visual_ready == false);

    frontend.runtime.view.camera_x = (uint16_t)(0x0986u - 48u);
    frontend.runtime.view.camera_y = (uint16_t)(512 - 0x005Fu - 32u);
    assert(dk1_barrel_scene_build_visual(&frontend.barrel, &rom,
                                         &scene.vram,
                                         frontend.runtime.view.camera_x,
                                         frontend.runtime.view.camera_y,
                                         frontend.player_vertical_origin,
                                         frontend.obsel));
    assert(frontend.barrel.visual.pieces == 7u);
    assert(frontend.barrel.visual.dma_bytes == 608u);

    dk1_rom_free(&owned);
    return 0;
}
