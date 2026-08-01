#include <assert.h>
#include <stdlib.h>
#include "dk1/level_object_spawn.h"

int main(void) {
    const char *path = getenv("DK1_TEST_ROM");
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom;
    Dk1LevelObjectSpawn spawn;
    if (path == NULL)
        path = "/mnt/data/Donkey Kong Country (USA) (Rev 2).sfc";
    assert(dk1_rom_load_file(path, &owned, &rom));
    assert(dk1_level_object_spawn_find(&rom, 0x0016u, 0x0023u,
                                       0u, 3u, &spawn));
    assert(spawn.record_index == 35u);
    assert(spawn.record.command == DK1_LEVEL_SPRITE_COMMAND_NORMAL);
    assert(spawn.record.source_pc == 0xBD96F4u);
    assert(spawn.record.world_x == 0x0986u);
    assert((uint16_t)spawn.record.world_y == 0x005Fu);
    assert(spawn.record.definition_address == 0x92A9u);
    assert(spawn.definition.type_id == 0x0023u);
    assert(spawn.scheduler_verified);
    assert(spawn.pass == DK1_OBJECT_PASS_PRIMARY_NORMAL);
    assert(spawn.callback_pc == 0xBFCF0Cu);
    assert(spawn.scheduler.type_id[3u] == 0x0023u);

    assert(dk1_level_object_spawn_first_common_barrel(
        &rom, 0x0016u, DK1_PRIMARY_OBJECT_FIRST_SLOT, &spawn));
    assert(spawn.record_index == 10u);
    assert(spawn.type_id == 0x0026u);
    assert(spawn.slot == DK1_PRIMARY_OBJECT_FIRST_SLOT);
    assert(spawn.callback_pc == 0xBFCF0Cu);
    assert(spawn.scheduler.type_id[DK1_PRIMARY_OBJECT_FIRST_SLOT] == 0x0026u);

    dk1_rom_free(&owned);
    return 0;
}
