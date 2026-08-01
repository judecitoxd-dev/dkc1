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
    Dk1GnawtyStepResult gnawty_result;
    size_t i;
    size_t gnawty_catalog_index = 0u;
    size_t maximum_simultaneous = 0u;
    bool gnawty_catalogued = false;
    bool multiple_found = false;
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
    assert(stats.object_stream_initialized);
    assert(frontend.level_objects_ready);
    assert(stats.stream_catalog_count ==
           frontend.level_objects.catalog_count);
    assert(stats.stream_active_count ==
           frontend.level_objects.active_count);
    assert(stats.stream_active_count <= DK1_PRIMARY_OBJECT_SLOT_COUNT);
    assert(stats.stream_overflow_count ==
           frontend.level_objects.overflow_count);
    assert(frontend.level_objects.callback_count ==
           frontend.level_objects.active_count);
    assert(frontend.level_objects.scheduler_verified);
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

    for (i = 0u; i < frontend.level_objects.catalog_count; ++i) {
        if (frontend.level_objects.entries[i].type_id ==
            DK1_OBJECT_TYPE_GNAWTY) {
            gnawty_catalogued = true;
            gnawty_catalog_index = i;
            break;
        }
    }
    assert(gnawty_catalogued);
    {
        const Dk1LevelObjectStreamEntry *source =
            &frontend.level_objects.entries[gnawty_catalog_index];
        const uint16_t camera_x = source->record.world_x > 48u ?
            (uint16_t)(source->record.world_x - 48u) : 0u;
        frontend.runtime.view.camera_x = camera_x;
        assert(dk1_level_object_stream_update(&frontend.level_objects,
                                               camera_x, 96u,
                                               128u, 128u));
        assert(dk1_software_frontend_sync_gnawty(&frontend));
        assert(frontend.gnawty_ready);
        assert(frontend.gnawty_record_index == source->record_index);
        assert(frontend.gnawty.type_id == DK1_OBJECT_TYPE_GNAWTY);
        assert(frontend.gnawty.motion.world_x == source->record.world_x);
        assert(frontend.gnawty.motion.world_y == source->record.world_y);
    }
    for (i = 0u; i < 32u && frontend.gnawty.animation.frame == 0u; ++i) {
        assert(dk1_gnawty_step(&frontend.gnawty, &rom,
                               frontend.player_terrain_ready ?
                                   &frontend.player_terrain : NULL,
                               NULL, &gnawty_result));
    }
    assert(frontend.gnawty.callback_verified);
    assert(frontend.gnawty.callback_pc == DK1_GNAWTY_CALLBACK_PC);
    assert(frontend.gnawty.animation.frame != 0u);
    assert(dk1_gnawty_build_visual(&frontend.gnawty, &rom,
                                    &scene.vram,
                                    frontend.runtime.view.camera_x,
                                    frontend.runtime.view.camera_y,
                                    frontend.player_vertical_origin,
                                    frontend.obsel));
    assert(frontend.gnawty.visual_ready);
    assert(frontend.gnawty.visual.pieces > 0u);
    assert(frontend.gnawty.visual.dma_bytes > 0u);

    {
        uint32_t camera_x;
        uint32_t maximum = (uint32_t)scene.camera.maximum_x + 384u;
        for (camera_x = 0u; camera_x <= maximum; camera_x += 16u) {
            size_t active_source_count = 0u;
            size_t ready_count = 0u;
            frontend.runtime.view.camera_x = (uint16_t)camera_x;
            assert(dk1_level_object_stream_update(&frontend.level_objects,
                                                   (uint16_t)camera_x,
                                                   384u, 128u, 128u));
            assert(dk1_software_frontend_sync_gnawty(&frontend));
            for (i = 0u; i < frontend.level_objects.catalog_count; ++i) {
                const Dk1LevelObjectStreamEntry *entry =
                    &frontend.level_objects.entries[i];
                if (entry->active &&
                    entry->type_id == DK1_OBJECT_TYPE_GNAWTY &&
                    entry->record_index <
                        DK1_LEVEL_OBJECT_STREAM_MAX_ENTRIES &&
                    !frontend.defeated_level_records[entry->record_index])
                    ++active_source_count;
            }
            if (frontend.gnawty_ready && frontend.gnawty.active)
                ++ready_count;
            for (i = 0u; i < DK1_SOFTWARE_FRONTEND_EXTRA_GNAWTIES; ++i) {
                if (frontend.additional_gnawties[i].ready) {
                    assert(frontend.additional_gnawties[i].runtime != NULL);
                    assert(frontend.additional_gnawties[i].runtime->active);
                    ++ready_count;
                }
            }
            assert(ready_count == frontend.gnawty_active_count);
            assert(ready_count == active_source_count);
            assert(frontend.gnawty_capacity_overflow_count == 0u);
            if (ready_count > maximum_simultaneous)
                maximum_simultaneous = ready_count;
            if (ready_count >= 2u)
                multiple_found = true;
        }
    }
    assert(multiple_found);
    assert(maximum_simultaneous >= 2u);
    assert(maximum_simultaneous <=
           DK1_SOFTWARE_FRONTEND_GNAWTY_CAPACITY);
    assert(DK1_SOFTWARE_FRONTEND_GNAWTY_CAPACITY ==
           DK1_PRIMARY_OBJECT_SLOT_COUNT);

    dk1_software_frontend_dispose(&frontend);
    dk1_rom_free(&owned);
    return 0;
}
