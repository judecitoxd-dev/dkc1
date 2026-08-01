#include "dk1/level_object_stream.h"
#include <string.h>

typedef struct StreamCapture {
    Dk1LevelObjectStream *stream;
} StreamCapture;

static bool entry_visible(const Dk1LevelObjectStreamEntry *entry,
                          uint16_t camera_x,
                          uint16_t viewport_width,
                          uint16_t left_margin,
                          uint16_t right_margin) {
    uint32_t low;
    uint32_t high;
    uint32_t x;
    if (entry == NULL || viewport_width == 0u)
        return false;
    low = camera_x > left_margin ?
        (uint32_t)camera_x - left_margin : 0u;
    high = (uint32_t)camera_x + viewport_width + right_margin;
    if (high > UINT16_MAX)
        high = UINT16_MAX;
    x = entry->record.world_x;
    return x >= low && x <= high;
}

static void capture_callback(size_t slot, Dk1ObjectPass pass,
                             uint32_t callback_pc, void *user_data) {
    StreamCapture *capture = (StreamCapture *)user_data;
    Dk1LevelObjectStream *stream;
    size_t i;
    if (capture == NULL || capture->stream == NULL)
        return;
    stream = capture->stream;
    for (i = 0u; i < stream->catalog_count; ++i) {
        Dk1LevelObjectStreamEntry *entry = &stream->entries[i];
        if (!entry->active || entry->slot != slot)
            continue;
        entry->pass = pass;
        entry->callback_pc = callback_pc;
        entry->callback_verified =
            callback_pc == entry->expected_callback_pc;
        ++stream->callback_count;
        return;
    }
}

bool dk1_level_object_stream_init(const Dk1RomImage *rom,
                                  uint16_t entrance_id,
                                  Dk1LevelObjectStream *stream) {
    size_t i;
    if (rom == NULL || stream == NULL)
        return false;
    memset(stream, 0, sizeof(*stream));
    stream->entrance_id = entrance_id;
    if (!dk1_level_sprite_list_read(rom, entrance_id, &stream->list))
        return false;

    for (i = 0u; i < stream->list.record_count; ++i) {
        const Dk1LevelSpriteRecord *record = &stream->list.records[i];
        Dk1LevelSpriteDefinition definition;
        Dk1ObjectTypeDispatch dispatch;
        Dk1LevelObjectStreamEntry *entry;
        if (record->command != DK1_LEVEL_SPRITE_COMMAND_NORMAL) {
            ++stream->non_normal_records;
            continue;
        }
        if (!dk1_level_sprite_definition_read(
                rom, record->definition_address, &definition) ||
            !definition.has_type_id) {
            ++stream->unresolved_records;
            continue;
        }
        if (definition.type_id == 0u ||
            !dk1_object_type_dispatch_get(definition.type_id, &dispatch)) {
            ++stream->unsupported_records;
            continue;
        }
        if (stream->catalog_count >= DK1_LEVEL_OBJECT_STREAM_MAX_ENTRIES)
            return false;
        entry = &stream->entries[stream->catalog_count++];
        entry->record = *record;
        entry->record_index = i;
        entry->type_id = definition.type_id;
        entry->expected_callback_pc =
            ((uint32_t)DK1_OBJECT_CALLBACK_BANK << 16u) |
            dispatch.callback_address_bf;
    }
    return true;
}

bool dk1_level_object_stream_update(Dk1LevelObjectStream *stream,
                                    uint16_t camera_x,
                                    uint16_t viewport_width,
                                    uint16_t left_margin,
                                    uint16_t right_margin) {
    bool slot_used[DK1_PRIMARY_OBJECT_SLOT_COUNT] = {false};
    bool slot_retained[DK1_PRIMARY_OBJECT_SLOT_COUNT] = {false};
    Dk1ObjectFrameState next_scheduler;
    StreamCapture capture;
    size_t i;
    if (stream == NULL || viewport_width == 0u)
        return false;

    stream->camera_x = camera_x;
    stream->viewport_width = viewport_width;
    stream->left_margin = left_margin;
    stream->right_margin = right_margin;
    stream->active_count = 0u;
    stream->entered_count = 0u;
    stream->retained_count = 0u;
    stream->exited_count = 0u;
    stream->overflow_count = 0u;
    stream->callback_count = 0u;
    stream->scheduler_verified = false;

    for (i = 0u; i < stream->catalog_count; ++i) {
        Dk1LevelObjectStreamEntry *entry = &stream->entries[i];
        entry->callback_pc = 0u;
        entry->callback_verified = false;
        if (!entry->active)
            continue;
        if (entry_visible(entry, camera_x, viewport_width,
                          left_margin, right_margin) &&
            entry->slot >= DK1_PRIMARY_OBJECT_FIRST_SLOT &&
            entry->slot < DK1_PRIMARY_OBJECT_FIRST_SLOT +
                              DK1_PRIMARY_OBJECT_SLOT_COUNT) {
            const size_t slot_index =
                entry->slot - DK1_PRIMARY_OBJECT_FIRST_SLOT;
            if (!slot_used[slot_index]) {
                slot_used[slot_index] = true;
                slot_retained[slot_index] = true;
                ++stream->retained_count;
                ++stream->active_count;
                continue;
            }
        }
        entry->active = false;
        entry->slot = 0u;
        ++stream->exited_count;
    }

    for (i = 0u; i < stream->catalog_count; ++i) {
        Dk1LevelObjectStreamEntry *entry = &stream->entries[i];
        size_t slot_index;
        if (entry->active ||
            !entry_visible(entry, camera_x, viewport_width,
                           left_margin, right_margin))
            continue;
        for (slot_index = 0u;
             slot_index < DK1_PRIMARY_OBJECT_SLOT_COUNT;
             ++slot_index) {
            if (!slot_used[slot_index])
                break;
        }
        if (slot_index == DK1_PRIMARY_OBJECT_SLOT_COUNT) {
            ++stream->overflow_count;
            continue;
        }
        slot_used[slot_index] = true;
        entry->active = true;
        entry->slot = DK1_PRIMARY_OBJECT_FIRST_SLOT + slot_index;
        ++stream->entered_count;
        ++stream->active_count;
    }

    next_scheduler = stream->scheduler;
    for (i = 0u; i < DK1_PRIMARY_OBJECT_SLOT_COUNT; ++i) {
        const size_t slot = DK1_PRIMARY_OBJECT_FIRST_SLOT + i;
        next_scheduler.type_id[slot] = 0u;
        if (!slot_retained[i])
            next_scheduler.state_1029[slot] = 0u;
    }
    for (i = 0u; i < stream->catalog_count; ++i) {
        const Dk1LevelObjectStreamEntry *entry = &stream->entries[i];
        if (entry->active)
            next_scheduler.type_id[entry->slot] = entry->type_id;
    }
    stream->scheduler = next_scheduler;

    capture.stream = stream;
    stream->frame_result = dk1_object_run_frame(
        &stream->scheduler, capture_callback, &capture);
    stream->scheduler_verified =
        stream->frame_result.secondary_callbacks == 0u &&
        stream->frame_result.primary_callbacks == stream->active_count &&
        stream->callback_count == stream->active_count;
    for (i = 0u; i < stream->catalog_count; ++i) {
        const Dk1LevelObjectStreamEntry *entry = &stream->entries[i];
        if (entry->active &&
            (!entry->callback_verified ||
             entry->pass != DK1_OBJECT_PASS_PRIMARY_NORMAL)) {
            stream->scheduler_verified = false;
            break;
        }
    }
    return stream->scheduler_verified;
}
