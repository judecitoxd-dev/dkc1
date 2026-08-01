#include <assert.h>
#include <stdlib.h>
#include "dk1/level_sprite_records.h"

int main(void) {
    const char *path = getenv("DK1_TEST_ROM");
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom;
    Dk1LevelSpriteList list;
    Dk1LevelSpriteRecord record;
    Dk1LevelSpriteDefinition definition;
    size_t index = 0u;
    uint16_t type = 0u;
    if (path == NULL)
        path = "/mnt/data/Donkey Kong Country (USA) (Rev 2).sfc";
    assert(dk1_rom_load_file(path, &owned, &rom));
    assert(dk1_level_sprite_list_read(&rom, 0x0016u, &list));
    assert(list.source_pc == 0xBD95DCu);
    assert(list.record_count == 66u && list.terminated);
    assert(list.records[35].command == DK1_LEVEL_SPRITE_COMMAND_NORMAL);
    assert(list.records[35].world_x == 0x0986u);
    assert((uint16_t)list.records[35].world_y == 0x005Fu);
    assert(list.records[35].definition_address == 0x92A9u);
    assert(dk1_level_sprite_definition_read(&rom, 0x92A9u, &definition));
    assert(definition.has_type_id && definition.type_id == 0x0023u);
    assert(definition.redirect_count == 1u);
    assert(dk1_level_sprite_record_type(&rom, &list.records[35], &type));
    assert(type == 0x0023u);
    assert(dk1_level_sprite_find_type(&rom, 0x0016u, 0x0023u, 0u,
                                      &record, &index));
    assert(index == 35u && record.source_pc == 0xBD96F4u);
    assert(record.world_x == 0x0986u);
    assert((uint16_t)record.world_y == 0x005Fu);
    assert(dk1_level_sprite_list_signature(&list) ==
           UINT64_C(0xBE8955E9C89E92DD));
    dk1_rom_free(&owned);
    return 0;
}
