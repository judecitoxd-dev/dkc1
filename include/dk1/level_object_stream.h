#ifndef DK1_LEVEL_OBJECT_STREAM_H
#define DK1_LEVEL_OBJECT_STREAM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/level_sprite_records.h"
#include "dk1/object_scheduler.h"

#define DK1_LEVEL_OBJECT_STREAM_MAX_ENTRIES DK1_LEVEL_SPRITE_MAX_RECORDS

typedef struct Dk1LevelObjectStreamEntry {
    Dk1LevelSpriteRecord record;
    size_t record_index;
    size_t slot;
    uint16_t type_id;
    uint32_t expected_callback_pc;
    uint32_t callback_pc;
    Dk1ObjectPass pass;
    bool active;
    bool callback_verified;
} Dk1LevelObjectStreamEntry;

typedef struct Dk1LevelObjectStream {
    Dk1LevelSpriteList list;
    Dk1ObjectFrameState scheduler;
    Dk1ObjectFrameResult frame_result;
    uint16_t entrance_id;
    uint16_t camera_x;
    uint16_t viewport_width;
    uint16_t left_margin;
    uint16_t right_margin;
    size_t catalog_count;
    size_t non_normal_records;
    size_t unresolved_records;
    size_t unsupported_records;
    size_t active_count;
    size_t entered_count;
    size_t retained_count;
    size_t exited_count;
    size_t overflow_count;
    size_t callback_count;
    bool scheduler_verified;
    Dk1LevelObjectStreamEntry
        entries[DK1_LEVEL_OBJECT_STREAM_MAX_ENTRIES];
} Dk1LevelObjectStream;

/* Builds a resolved catalog for every supported normal record in an entrance. */
bool dk1_level_object_stream_init(const Dk1RomImage *rom,
                                  uint16_t entrance_id,
                                  Dk1LevelObjectStream *stream);

/* Retains visible objects in stable primary slots, removes records outside the
 * camera envelope, fills free slots in original list order, and verifies one
 * scheduler pass. The original primary pool limit of 25 is enforced. */
bool dk1_level_object_stream_update(Dk1LevelObjectStream *stream,
                                    uint16_t camera_x,
                                    uint16_t viewport_width,
                                    uint16_t left_margin,
                                    uint16_t right_margin);

#endif
