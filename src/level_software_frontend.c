#include "dk1/level_software_frontend.h"
#include <string.h>

bool dk1_level_software_frontend_init(const Dk1RomImage *rom,
                                      const Dk1SceneMemory *scene,
                                      uint16_t entrance_id,
                                      uint16_t width,
                                      uint16_t height,
                                      Dk1SoftwareFrontend *frontend,
                                      Dk1LevelSoftwareFrontendStats *stats) {
    Dk1LevelSoftwareFrontendStats local_stats;
    Dk1LevelSoftwareFrontendStats *output =
        stats != NULL ? stats : &local_stats;
    if (rom == NULL || scene == NULL || frontend == NULL) return false;
    memset(output, 0, sizeof(*output));
    output->entrance_id = entrance_id;
    if (!dk1_software_frontend_init_with_rom(rom, scene, width, height,
                                             frontend))
        return false;

    output->object_list_imported = dk1_level_object_import_primary(
        rom, entrance_id, &output->primary_import);

    frontend->level_objects_ready =
        dk1_level_object_stream_init(
            rom, entrance_id, &frontend->level_objects) &&
        dk1_level_object_stream_update(
            &frontend->level_objects, frontend->runtime.view.camera_x,
            frontend->runtime.view.width, 128u, 128u);
    output->object_stream_initialized = frontend->level_objects_ready;
    if (frontend->level_objects_ready) {
        output->stream_catalog_count =
            frontend->level_objects.catalog_count;
        output->stream_active_count =
            frontend->level_objects.active_count;
        output->stream_overflow_count =
            frontend->level_objects.overflow_count;
        if (!dk1_software_frontend_sync_gnawty(frontend))
            return false;
        output->gnawty_spawned = frontend->gnawty_ready;
        if (frontend->gnawty_ready)
            output->gnawty_record_index = frontend->gnawty_record_index;
    }

    if (!dk1_level_object_spawn_find(
            rom, entrance_id, DK1_OBJECT_TYPE_BARREL, 0u,
            DK1_PRIMARY_OBJECT_FIRST_SLOT, &output->barrel_spawn))
        return dk1_software_frontend_sync_barrels(frontend);
    output->barrel_found = true;
    if (!dk1_software_frontend_spawn_barrel(
            frontend, output->barrel_spawn.type_id,
            output->barrel_spawn.record.world_x,
            output->barrel_spawn.record.world_y))
        return false;
    frontend->barrel_record_index = output->barrel_spawn.record_index;
    output->barrel_spawned = true;
    return dk1_software_frontend_sync_barrels(frontend);
}
