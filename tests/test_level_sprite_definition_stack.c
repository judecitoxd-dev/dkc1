#include <assert.h>
#include <stdlib.h>
#include "dk1/level_sprite_records.h"

static void write_u16(uint8_t *data, size_t size, uint32_t pc, uint16_t value) {
    size_t offset;
    assert(dk1_hirom_offset(pc, size, &offset));
    assert(offset + 1u < size);
    data[offset] = (uint8_t)value;
    data[offset + 1u] = (uint8_t)(value >> 8u);
}

static bool assignment_value(const Dk1LevelSpriteDefinition *definition,
                             uint16_t field, uint16_t *value) {
    size_t i;
    for (i = 0u; i < definition->assignment_count; ++i) {
        if (definition->assignments[i].field_address == field) {
            if (value != NULL) *value = definition->assignments[i].value;
            return true;
        }
    }
    return false;
}

int main(void) {
    uint8_t *data = calloc(DK1_EXPECTED_ROM_SIZE, 1u);
    Dk1RomImage rom;
    Dk1LevelSpriteDefinition definition;
    uint16_t value;
    assert(data != NULL);
    assert(dk1_rom_image_init(&rom, data, DK1_EXPECTED_ROM_SIZE));

    /* $9000 calls $9100, then resumes and overwrites the type. */
    write_u16(data, DK1_EXPECTED_ROM_SIZE, 0xB59000u, 0x8200u);
    write_u16(data, DK1_EXPECTED_ROM_SIZE, 0xB59002u, 0x9100u);
    write_u16(data, DK1_EXPECTED_ROM_SIZE, 0xB59004u,
              DK1_LEVEL_SPRITE_TYPE_FIELD);
    write_u16(data, DK1_EXPECTED_ROM_SIZE, 0xB59006u, 0x0022u);
    write_u16(data, DK1_EXPECTED_ROM_SIZE, 0xB59008u, 0x0FF5u);
    write_u16(data, DK1_EXPECTED_ROM_SIZE, 0xB5900Au, 0xBEEFu);
    write_u16(data, DK1_EXPECTED_ROM_SIZE, 0xB5900Cu, 0x8000u);
    write_u16(data, DK1_EXPECTED_ROM_SIZE, 0xB5900Eu, 0u);

    write_u16(data, DK1_EXPECTED_ROM_SIZE, 0xB59100u,
              DK1_LEVEL_SPRITE_TYPE_FIELD);
    write_u16(data, DK1_EXPECTED_ROM_SIZE, 0xB59102u, 0x0023u);
    write_u16(data, DK1_EXPECTED_ROM_SIZE, 0xB59104u, 0x8000u);
    write_u16(data, DK1_EXPECTED_ROM_SIZE, 0xB59106u, 0u);

    assert(dk1_level_sprite_definition_read(&rom, 0x9000u, &definition));
    assert(definition.redirect_count == 1u);
    assert(definition.has_type_id && definition.type_id == 0x0022u);
    assert(assignment_value(&definition, DK1_LEVEL_SPRITE_TYPE_FIELD, &value));
    assert(value == 0x0022u);
    assert(assignment_value(&definition, 0x0FF5u, &value));
    assert(value == 0xBEEFu);
    assert(definition.terminal_command == 0u);

    /* Current call-stack detection rejects a recursive loop. */
    write_u16(data, DK1_EXPECTED_ROM_SIZE, 0xB59200u, 0x8200u);
    write_u16(data, DK1_EXPECTED_ROM_SIZE, 0xB59202u, 0x9200u);
    assert(!dk1_level_sprite_definition_read(&rom, 0x9200u, &definition));

    free(data);
    return 0;
}
