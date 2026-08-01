#include <assert.h>
#include <stdlib.h>
#include "dk1/level_object_import.h"

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
    Dk1LevelObjectImport import_state;
    size_t i;
    assert(data != NULL);
    assert(dk1_rom_image_init(&rom, data, DK1_EXPECTED_ROM_SIZE));

    write_u16(data, DK1_EXPECTED_ROM_SIZE,
              DK1_LEVEL_SPRITE_POINTER_TABLE, 0x9000u);
    for (i = 0u; i < 27u; ++i) {
        const uint32_t record_pc =
            0xBD9000u + (uint32_t)i * 8u;
        const uint16_t definition_address =
            (uint16_t)(0xA000u + (uint16_t)i * 8u);
        const uint32_t definition_pc =
            0xB50000u | definition_address;
        write_u16(data, DK1_EXPECTED_ROM_SIZE,
                  record_pc, DK1_LEVEL_SPRITE_COMMAND_NORMAL);
        write_u16(data, DK1_EXPECTED_ROM_SIZE,
                  record_pc + 2u, (uint16_t)(0x0100u + i));
        write_u16(data, DK1_EXPECTED_ROM_SIZE,
                  record_pc + 4u, (uint16_t)(0x0020u + i));
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
              0xBD9000u + 27u * 8u, 0u);

    assert(dk1_level_object_import_primary(&rom, 0u, &import_state));
    assert(import_state.list.terminated);
    assert(import_state.list.record_count == 27u);
    assert(import_state.normal_records == 27u);
    assert(import_state.non_normal_records == 0u);
    assert(import_state.unresolved_records == 0u);
    assert(import_state.unsupported_records == 0u);
    assert(import_state.imported_count ==
           DK1_PRIMARY_OBJECT_SLOT_COUNT);
    assert(import_state.overflow_records == 2u);
    assert(import_state.callback_count ==
           DK1_PRIMARY_OBJECT_SLOT_COUNT);
    assert(import_state.frame_result.secondary_callbacks == 0u);
    assert(import_state.frame_result.primary_callbacks ==
           DK1_PRIMARY_OBJECT_SLOT_COUNT);
    assert(import_state.scheduler_verified);

    assert(import_state.entries[0].record_index == 0u);
    assert(import_state.entries[0].slot ==
           DK1_PRIMARY_OBJECT_FIRST_SLOT);
    assert(import_state.entries[0].type_id == 0x0023u);
    assert(import_state.entries[0].pass ==
           DK1_OBJECT_PASS_PRIMARY_NORMAL);
    assert(import_state.entries[0].callback_pc == 0xBFCF0Cu);
    assert(import_state.entries[0].callback_verified);

    assert(import_state.entries[24].record_index == 24u);
    assert(import_state.entries[24].slot == 25u);
    assert(import_state.entries[24].callback_pc == 0xBFCF0Cu);
    assert(import_state.entries[24].callback_verified);
    assert(import_state.scheduler.type_id[1u] == 0x0023u);
    assert(import_state.scheduler.type_id[25u] == 0x0023u);
    assert(import_state.scheduler.type_id[26u] == 0u);

    free(data);
    return 0;
}
