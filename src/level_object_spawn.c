#include "dk1/level_object_spawn.h"
#include <string.h>

#define DK1_COMMON_BARREL_CALLBACK_ADDRESS 0xCF0Cu

typedef struct Capture {
    size_t slot;
    Dk1ObjectPass pass;
    uint32_t pc;
    size_t count;
} Capture;

static void capture(size_t slot, Dk1ObjectPass pass, uint32_t pc,
                    void *user_data) {
    Capture *capture_state = (Capture *)user_data;
    if (capture_state == NULL) return;
    if (capture_state->count++ == 0u) {
        capture_state->slot = slot;
        capture_state->pass = pass;
        capture_state->pc = pc;
    }
}

static bool finish_spawn(const Dk1RomImage *rom,
                         uint16_t entrance_id,
                         uint16_t type_id,
                         size_t record_index,
                         const Dk1LevelSpriteRecord *record,
                         size_t slot,
                         Dk1LevelObjectSpawn *spawn) {
    Capture capture_state = {0};
    Dk1ObjectFrameResult frame_result;
    Dk1ObjectTypeDispatch dispatch;
    if (rom == NULL || record == NULL || spawn == NULL ||
        slot >= DK1_OBJECT_SLOT_COUNT ||
        record->command != DK1_LEVEL_SPRITE_COMMAND_NORMAL ||
        !dk1_object_type_dispatch_get(type_id, &dispatch))
        return false;
    memset(spawn, 0, sizeof(*spawn));
    spawn->record = *record;
    if (!dk1_level_sprite_definition_read(rom, record->definition_address,
                                          &spawn->definition) ||
        !spawn->definition.has_type_id ||
        spawn->definition.type_id != type_id)
        return false;
    spawn->entrance_id = entrance_id;
    spawn->type_id = type_id;
    spawn->record_index = record_index;
    spawn->slot = slot;
    spawn->scheduler.type_id[slot] = type_id;
    frame_result = dk1_object_run_frame(&spawn->scheduler, capture,
                                         &capture_state);
    if (frame_result.primary_callbacks + frame_result.secondary_callbacks != 1u ||
        capture_state.count != 1u || capture_state.slot != slot)
        return false;
    spawn->pass = capture_state.pass;
    spawn->callback_pc = capture_state.pc;
    spawn->scheduler_verified =
        capture_state.pc == (((uint32_t)DK1_OBJECT_CALLBACK_BANK << 16u) |
                             dispatch.callback_address_bf);
    return spawn->scheduler_verified;
}

bool dk1_level_object_spawn_find(const Dk1RomImage *rom,
                                 uint16_t entrance_id,
                                 uint16_t type_id,
                                 size_t occurrence,
                                 size_t slot,
                                 Dk1LevelObjectSpawn *spawn) {
    Dk1LevelSpriteRecord record;
    size_t record_index;
    if (rom == NULL || spawn == NULL ||
        !dk1_level_sprite_find_type(rom, entrance_id, type_id, occurrence,
                                    &record, &record_index))
        return false;
    return finish_spawn(rom, entrance_id, type_id, record_index, &record,
                        slot, spawn);
}

bool dk1_level_object_spawn_first_common_barrel(const Dk1RomImage *rom,
                                                 uint16_t entrance_id,
                                                 size_t slot,
                                                 Dk1LevelObjectSpawn *spawn) {
    Dk1LevelSpriteList list;
    size_t i;
    if (rom == NULL || spawn == NULL ||
        !dk1_level_sprite_list_read(rom, entrance_id, &list))
        return false;
    for (i = 0u; i < list.record_count; ++i) {
        uint16_t type_id;
        Dk1ObjectTypeDispatch dispatch;
        const Dk1LevelSpriteRecord *record = &list.records[i];
        if (record->command != DK1_LEVEL_SPRITE_COMMAND_NORMAL ||
            !dk1_level_sprite_record_type(rom, record, &type_id) ||
            !dk1_object_type_dispatch_get(type_id, &dispatch) ||
            dispatch.callback_address_bf != DK1_COMMON_BARREL_CALLBACK_ADDRESS)
            continue;
        return finish_spawn(rom, entrance_id, type_id, i, record, slot, spawn);
    }
    return false;
}
