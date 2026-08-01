#ifndef DK1_LEVEL_OBJECT_IMPORT_H
#define DK1_LEVEL_OBJECT_IMPORT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/level_sprite_records.h"
#include "dk1/object_scheduler.h"

#define DK1_LEVEL_OBJECT_IMPORT_MAX_ENTRIES DK1_PRIMARY_OBJECT_SLOT_COUNT

typedef struct Dk1LevelObjectImportEntry {
    Dk1LevelSpriteRecord record;
    Dk1LevelSpriteDefinition definition;
    size_t record_index;
    size_t slot;
    uint16_t type_id;
    uint32_t expected_callback_pc;
    uint32_t callback_pc;
    Dk1ObjectPass pass;
    bool callback_verified;
} Dk1LevelObjectImportEntry;

typedef struct Dk1LevelObjectImport {
    Dk1LevelSpriteList list;
    Dk1ObjectFrameState scheduler;
    Dk1ObjectFrameResult frame_result;
    uint16_t entrance_id;
    size_t normal_records;
    size_t non_normal_records;
    size_t unresolved_records;
    size_t unsupported_records;
    size_t overflow_records;
    size_t imported_count;
    size_t callback_count;
    bool scheduler_verified;
    Dk1LevelObjectImportEntry
        entries[DK1_LEVEL_OBJECT_IMPORT_MAX_ENTRIES];
} Dk1LevelObjectImport;

/* Imports normal records in original list order into the bounded primary object
 * pool, then executes one scheduler pass to verify every callback. Records that
 * cannot yet be resolved and records beyond the 25-slot primary pool are kept
 * in the accounting fields instead of being silently discarded. */
bool dk1_level_object_import_primary(const Dk1RomImage *rom,
                                     uint16_t entrance_id,
                                     Dk1LevelObjectImport *import_state);

#endif
