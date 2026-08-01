#include <stdio.h>
#include <stdlib.h>
#include "dk1/level_software_frontend.h"
#include "dk1/rom_image.h"
#include "dk1/scene_memory.h"
#include "dk1/scene_signature.h"

int main(int argc, char **argv) {
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom = {0};
    Dk1RomIdentity identity;
    Dk1SceneMemory *scene = NULL;
    Dk1Rgba8 *pixels = NULL;
    Dk1RgbaSurface surface;
    uint64_t hash = 0u;
    uint16_t level;
    size_t failed = 0u;
    size_t terrain = 0u;
    size_t local = 0u;
    size_t object_lists = 0u;
    size_t object_records = 0u;
    size_t object_imports = 0u;
    size_t object_unresolved = 0u;
    size_t object_overflow = 0u;
    size_t object_streams = 0u;
    size_t stream_catalog = 0u;
    size_t stream_active = 0u;
    size_t stream_entered = 0u;
    size_t stream_exited = 0u;
    size_t stream_overflow = 0u;
    size_t barrel_records = 0u;
    size_t barrel_spawns = 0u;

    if (argc != 2) return 2;
    if (!dk1_rom_load_file(argv[1], &owned, &rom) ||
        !dk1_rom_identity(&rom, &identity) ||
        !dk1_rom_identity_is_supported_rev2(&identity))
        return 1;

    scene = (Dk1SceneMemory *)malloc(sizeof(*scene));
    pixels = (Dk1Rgba8 *)calloc(96u * 64u, sizeof(*pixels));
    if (scene == NULL || pixels == NULL) {
        free(scene);
        free(pixels);
        dk1_rom_free(&owned);
        return 1;
    }
    surface = (Dk1RgbaSurface){pixels, 96u, 64u, 96u};

    for (level = 0u; level < DK1_SCENE_LEVEL_COUNT; ++level) {
        Dk1SoftwareFrontend frontend;
        Dk1LevelSoftwareFrontendStats source_stats;
        size_t i;
        if (!dk1_scene_memory_load(&rom, level, false, false, scene) ||
            !dk1_level_software_frontend_init(&rom, scene, level,
                                              96u, 64u,
                                              &frontend, &source_stats) ||
            !dk1_software_frontend_step(scene, DK1_HOST_BUTTON_B,
                                        &frontend) ||
            !dk1_software_frontend_render(&rom, scene, &frontend, surface)) {
            ++failed;
            continue;
        }
        if (frontend.player_terrain_ready) ++terrain;
        if (frontend.player_live.used_local_semantics) ++local;
        if (source_stats.object_list_imported) {
            const Dk1LevelObjectImport *import_state =
                &source_stats.primary_import;
            const uint64_t list_signature =
                dk1_level_sprite_list_signature(&import_state->list);
            ++object_lists;
            object_records += import_state->list.record_count;
            object_imports += import_state->imported_count;
            object_unresolved += import_state->unresolved_records +
                                 import_state->unsupported_records;
            object_overflow += import_state->overflow_records;
            hash = dk1_fnv1a64(&list_signature,
                               sizeof(list_signature), hash);
            hash = dk1_fnv1a64(&import_state->imported_count,
                               sizeof(import_state->imported_count), hash);
            hash = dk1_fnv1a64(&import_state->overflow_records,
                               sizeof(import_state->overflow_records), hash);
            for (i = 0u; i < import_state->imported_count; ++i) {
                const Dk1LevelObjectImportEntry *entry =
                    &import_state->entries[i];
                hash = dk1_fnv1a64(&entry->record,
                                   sizeof(entry->record), hash);
                hash = dk1_fnv1a64(&entry->type_id,
                                   sizeof(entry->type_id), hash);
                hash = dk1_fnv1a64(&entry->callback_pc,
                                   sizeof(entry->callback_pc), hash);
            }
        }
        if (frontend.level_objects_ready) {
            const Dk1LevelObjectStream *stream = &frontend.level_objects;
            ++object_streams;
            stream_catalog += stream->catalog_count;
            stream_active += stream->active_count;
            stream_entered += stream->entered_count;
            stream_exited += stream->exited_count;
            stream_overflow += stream->overflow_count;
            hash = dk1_fnv1a64(&stream->camera_x,
                               sizeof(stream->camera_x), hash);
            hash = dk1_fnv1a64(&stream->active_count,
                               sizeof(stream->active_count), hash);
            hash = dk1_fnv1a64(&stream->overflow_count,
                               sizeof(stream->overflow_count), hash);
            for (i = 0u; i < stream->catalog_count; ++i) {
                const Dk1LevelObjectStreamEntry *entry = &stream->entries[i];
                if (!entry->active)
                    continue;
                hash = dk1_fnv1a64(&entry->record_index,
                                   sizeof(entry->record_index), hash);
                hash = dk1_fnv1a64(&entry->slot,
                                   sizeof(entry->slot), hash);
                hash = dk1_fnv1a64(&entry->type_id,
                                   sizeof(entry->type_id), hash);
                hash = dk1_fnv1a64(&entry->callback_pc,
                                   sizeof(entry->callback_pc), hash);
            }
        }
        if (source_stats.barrel_found) ++barrel_records;
        if (source_stats.barrel_spawned) ++barrel_spawns;
        hash = dk1_fnv1a64(&level, sizeof(level), hash);
        hash = dk1_fnv1a64(&frontend.runtime, sizeof(frontend.runtime), hash);
        hash = dk1_fnv1a64(&frontend.player_preview,
                           sizeof(frontend.player_preview), hash);
        hash = dk1_fnv1a64(&frontend.player_live,
                           sizeof(frontend.player_live), hash);
        hash = dk1_fnv1a64(&source_stats.barrel_found,
                           sizeof(source_stats.barrel_found), hash);
        if (source_stats.barrel_found) {
            hash = dk1_fnv1a64(&source_stats.barrel_spawn.record,
                               sizeof(source_stats.barrel_spawn.record), hash);
            hash = dk1_fnv1a64(&source_stats.barrel_spawn.callback_pc,
                               sizeof(source_stats.barrel_spawn.callback_pc),
                               hash);
        }
        hash = dk1_fnv1a64(pixels, 96u * 64u * sizeof(*pixels), hash);
    }

    printf("frontends=%u failed=%zu terrain=%zu local_dispatch=%zu "
           "object_lists=%zu object_records=%zu object_imports=%zu "
           "object_unresolved=%zu object_overflow=%zu "
           "object_streams=%zu stream_catalog=%zu stream_active=%zu "
           "stream_entered=%zu stream_exited=%zu stream_overflow=%zu "
           "barrel_records=%zu barrel_spawns=%zu signature=%016llX\n",
           DK1_SCENE_LEVEL_COUNT, failed, terrain, local,
           object_lists, object_records, object_imports,
           object_unresolved, object_overflow,
           object_streams, stream_catalog, stream_active,
           stream_entered, stream_exited, stream_overflow,
           barrel_records, barrel_spawns, (unsigned long long)hash);
    free(scene);
    free(pixels);
    dk1_rom_free(&owned);
    return failed != 0u ? 1 : 0;
}
