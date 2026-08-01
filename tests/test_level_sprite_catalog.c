#include <assert.h>
#include <stdlib.h>
#include "dk1/level_sprite_records.h"

static uint64_t hash_byte(uint64_t hash, uint8_t value) {
    hash ^= value;
    return hash * UINT64_C(1099511628211);
}

static uint64_t hash_word(uint64_t hash, uint16_t value) {
    hash = hash_byte(hash, (uint8_t)value);
    return hash_byte(hash, (uint8_t)(value >> 8u));
}

int main(void) {
    const char *path = getenv("DK1_TEST_ROM");
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom;
    uint64_t signature = UINT64_C(14695981039346656037);
    size_t normal = 0u, resolved = 0u, unresolved = 0u;
    uint16_t entrance;
    if (path == NULL)
        path = "/mnt/data/Donkey Kong Country (USA) (Rev 2).sfc";
    assert(dk1_rom_load_file(path, &owned, &rom));

    for (entrance = 0u; entrance < 230u; ++entrance) {
        Dk1LevelSpriteList list;
        size_t i;
        assert(dk1_level_sprite_list_read(&rom, entrance, &list));
        for (i = 0u; i < list.record_count; ++i) {
            const Dk1LevelSpriteRecord *record = &list.records[i];
            uint16_t type = UINT16_MAX;
            if (record->command != DK1_LEVEL_SPRITE_COMMAND_NORMAL) continue;
            ++normal;
            if (dk1_level_sprite_record_type(&rom, record, &type)) ++resolved;
            else ++unresolved;
            signature = hash_word(signature, entrance);
            signature = hash_word(signature, (uint16_t)i);
            signature = hash_word(signature, record->world_x);
            signature = hash_word(signature, (uint16_t)record->world_y);
            signature = hash_word(signature, record->definition_address);
            signature = hash_word(signature, type);
        }
    }

    assert(normal == 10357u);
    assert(resolved == 10356u);
    assert(unresolved == 1u);
    assert(signature == UIN64_C(0xA5702FF2DA67FE40));
    dk1_rom_free(&owned);
    return 0;
}
