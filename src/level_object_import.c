#include "dk1/level_object_import.h"
#include <string.h>

typedef struct ImportCapture {
    Dk1LevelObjectImport *import_state;
} ImportCapture;

static void capture_callback(size_t slot, Dk1ObjectPass pass,
                             uint32_t callback_pc, void *user_data) {
    ImportCapture *capture = (ImportCapture *)user_data;
    Dk1LevelObjectImport *import_state;
    Dk1LevelObjectImportEntry *entry;
    size_t entry_index;
    if (capture == NULL || capture->import_state == NULL ||
        slot < DK1_PRIMARY_OBJECT_FIRST_SLOT)
        return;
    import_state = capture->import_state;
    entry_index = slot - DK1_PRIMARY_OBJECT_FIRST_SLOT;
    if (entry_index >= import_state->imported_count)
        return;
    entry = &import_state->entries[entry_index];
    if (entry->slot != slot)
        return;
    entry->pass = pass;
    entry->callback_pc = callback_pc;
    entry->callback_verified = callback_pc == entry->expected_callback_pc;
    ++import_state->callback_count;
}

bool dk1_level_object_import_primary(const Dk1RomImage *rom,
                                     uint16_t entrance_id,
                                     Dk1LevelObjectImport *import_state) {
    ImportCapture capture;
    size_t i;
    if (rom == NULL || import_state == NULL)
        return false;
    memset(import_state, 0, sizeof(*import_state));
    import_state->entrance_id = entrance_id;
    if (!dk1_level_sprite_list_read(rom, entrance_id, &import_state->list))
        return false;

    for (i = 0u; i < import_state->list.record_count; ++i) {
        const Dk1LevelSpriteRecord *record = &import_state->list.records[i];
        Dk1LevelSpriteDefinition definition;
        Dk1ObjectTypeDispatch dispatch;
        Dk1LevelObjectImportEntry *entry;
        size_t slot;
        if (record->command != DK1_LEVEL_SPRITE_COMMAND_NORMAL) {
            ++import_state->non_normal_records;
            continue;
        }
        ++import_state->normal_records;
        if (!dk1_level_sprite_definition_read(
                rom, record->definition_address, &definition) ||
            !definition.has_type_id) {
            ++import_state->unresolved_records;
            continue;
        }
        if (definition.type_id == 0u ||
            !dk1_object_type_dispatch_get(definition.type_id, &dispatch)) {
            ++import_state->unsupported_records;
            continue;
        }
        if (import_state->imported_count >=
            DK1_LEVEL_OBJECT_IMPORT_MAX_ENTRIES) {
            ++import_state->overflow_records;
            continue;
        }

        slot = DK1_PRIMARY_OBJECT_FIRST_SLOT + import_state->imported_count;
        entry = &import_state->entries[import_state->imported_count];
        entry->record = *record;
        entry->definition = definition;
        entry->record_index = i;
        entry->slot = slot;
        entry->type_id = definition.type_id;
        entry->expected_callback_pc =
            ((uint32_t)DK1_OBJECT_CALLBACK_BANK << 16u) |
            dispatch.callback_address_bf;
        import_state->scheduler.type_id[slot] = definition.type_id;
        ++import_state->imported_count;
    }

    capture.import_state = import_state;
    import_state->frame_result =
        dk1_object_run_frame(&import_state->scheduler,
                             capture_callback, &capture);
    import_state->scheduler_verified =
        import_state->frame_result.secondary_callbacks == 0u &&
        import_state->frame_result.primary_callbacks ==
            import_state->imported_count &&
        import_state->callback_count == import_state->imported_count;
    for (i = 0u; i < import_state->imported_count; ++i) {
        if (!import_state->entries[i].callback_verified) {
            import_state->scheduler_verified = false;
            break;
        }
    }
    return import_state->scheduler_verified;
}
