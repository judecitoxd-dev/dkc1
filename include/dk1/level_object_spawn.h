#ifndef DK1_LEVEL_OBJECT_SPAWN_H
#define DK1_LEVEL_OBJECT_SPAWN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/level_sprite_records.h"
#include "dk1/object_scheduler.h"

typedef struct Dk1LevelObjectSpawn {
    Dk1LevelSpriteRecord record;
    Dk1LevelSpriteDefinition definition;
    Dk1ObjectFrameState scheduler;
    uint16_t entrance_id;
    uint16_t type_id;
    size_t record_index;
    size_t slot;
    Dk1ObjectPass pass;
    uint32_t callback_pc;
    bool scheduler_verified;
} Dk1LevelObjectSpawn;

bool dk1_level_object_spawn_find(const Dk1RomImage *rom,
                                 uint16_t entrance_id,
                                 uint16_t type_id,
                                 size_t occurrence,
                                 size_t slot,
                                 Dk1LevelObjectSpawn *spawn);

/* Finds the first normal level record whose type dispatches through the common
 * barrel callback $BF:CF0C, preserving original list order. */
bool dk1_level_object_spawn_first_common_barrel(const Dk1RomImage *rom,
                                                 uint16_t entrance_id,
                                                 size_t slot,
                                                 Dk1LevelObjectSpawn *spawn);

#endif
