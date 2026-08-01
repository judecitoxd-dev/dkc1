#include <assert.h>
#include <stdlib.h>
#include "dk1/level_object_stream.h"

static void write_u16(uint8_t *data, size_t size,
                      uint32_t snes_pc, uint16_t value) {
    size_t offset = 0u;
    assert(dk1_hirom_offset(snes_pc, size, &offset));
    assert(offset + 1u < size);
    data[offset] = (uint8_t)value;
    data[offset + 1u] = (uint8_t)(value >> 8u);
}

int main(void) {
    uint8_t *data = (uint8_t *)calloc(DK1_EXPECTED_ROM_SIZE, 1u);
    Dk1RomImage rom;
    Dk1LevelObjectStream stream;
    size_t i;
    assert(data != NULL);
    assert(dk1_rom_image_init(&rom, data, DK1_EXPECTED_ROM_SIZE));

    write_u16(data, DK1_EXPECTED_ROM_SIZE,
              DK1_LEVEL_SPRITE_POINTER_TABLE, 0x9000u);
    for (i = 0u; i < 30u; ++i) {
        const uint32_t record_pc =
            0xBD9000u + (uint32_t)i * 8u;
        const uint16_t definition_address =
            (uint16_t)(0xA000u + (uint16_t)i * 8u);
        const uint32_t definition_pc =
            0xB50000u | definition_address;
        write_u16(data, DK1_EXPECTED_ROM_SIZE,
                  record_pc, DK1_LEVEL_SPRITE_COMMAND_NORMAL);
        write_u16(data, DK1_EXPECTED_ROM_SIZE,
                  record_pc + 2u, (uint16_t)(i * 100u));
        write_u16(data, DK1_EXPECTED_ROM_SIZE,
                  record_pc + 4u, 0x0040u);
        write_u16(data, DK1_EXPECTED_ROM_SIZE,
                  record_pc + 6u, definition_address);
        write_u16(data, DK1_EXPECTED_ROM_SIZE,
                  definition_pc, DK1_LEVEL_SPRITE_TYPE_FIELD);
        write_u16(data, DK1_EXPECTED_ROM_SIZE,
                  definition_pc + 2u, 0x0023u);
        write_u16(data, DK1_EXPECTED_ROM_SIZE,
                  definition_pc + 4u, 0x8000u);
        write_u16(data, DK1_EXPECTED_ROM_SIZE,
                  definition_pc + 6u, 0u);
    }
    write_u16(data, DK1_EXPECTED_ROM_SIZE,
              0xBD9000u + 30u * 8u, 0u);

    assert(dk1_level_object_stream_init(&rom, 0u, &stream));
    assert(stream.catalog_count == 30u);
    assert(stream.non_normal_records == 0u);
    assert(stream.unresolved_records == 0u);
    assert(stream.unsupported_records == 0u);

    assert(dk1_level_object_stream_update(&stream, 0u, 250u, 0u, 0u));
    assert(stream.active_count == 3u);
    assert(stream.entered_count == 3u);
    assert(stream.retained_count == 0u);
    assert(stream.exited_count == 0u);
    assert(stream.overflow_count == 0u);
    assert(stream.callback_count == 3u);
    assert(stream.entries[0].active && stream.entries[0].slot == 1u);
    assert(stream.entries[1].active && stream.entries[1].slot == 2u);
    assert(stream.entries[2].active && stream.entries[2].slot == 3u);

    assert(dk1_level_object_stream_update(&stream, 100u, 250u, 0u, 0u));
    assert(stream.active_count == 3u);
    assert(stream.entered_count == 1u);
    assert(stream.retained_count == 2u);
    assert(stream.exited_count == 1u);
    assert(!stream.entries[0].active);
    assert(stream.entries[1].active && stream.entries[1].slot == 2u);
    assert(stream.entries[2].active && stream.entries[2].slot == 3u);
    assert(stream.entries[3].active && stream.entries[3].slot == 1u);
    assert(stream.entries[3].callback_pc == 0xBFCF0Cu);
    assert(stream.entries[3].callback_verified);

    assert(dk1_level_object_stream_update(&stream, 0u, 3000u, 0u, 0u));
    assert(stream.active_count == DK1_PRIMARY_OBJECT_SLOT_COUNT);
    assert(stream.retained_count == 3u);
    assert(stream.entered_count == 22u);
    assert(stream.exited_count == 0u);
    assert(stream.overflow_count == 5u);
    assert(stream.callback_count == DK1_PRIMARY_OBJECT_SLOT_COUNT);
    assert(stream.frame_result.primary_callbacks ==
           DK1_PRIMARY_OBJECT_SLOT_COUNT);
    assert(stream.scheduler_verified);
    assert(stream.entries[1].slot == 2u);
    assert(stream.entries[2].slot == 3u);
    assert(stream.entries[3].slot == 1u);

    free(data);
    return 0;
}
