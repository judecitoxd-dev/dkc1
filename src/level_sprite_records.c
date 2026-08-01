#include "dk1/level_sprite_records.h"
#include <string.h>

static uint32_t bank_pc(uint8_t bank, uint16_t address) {
    return ((uint32_t)bank << 16u) | address;
}

static bool assignment_set(Dk1LevelSpriteDefinition *definition,
                           uint16_t field, uint16_t value) {
    size_t i;
    for (i = 0u; i < definition->assignment_count; ++i) {
        if (definition->assignments[i].field_address == field) {
            definition->assignments[i].value = value;
            if (field == DK1_LEVEL_SPRITE_TYPE_FIELD) {
                definition->has_type_id = true;
                definition->type_id = value;
            }
            return true;
        }
    }
    if (definition->assignment_count >= DK1_LEVEL_SPRITE_MAX_ASSIGNMENTS)
        return false;
    definition->assignments[definition->assignment_count].field_address = field;
    definition->assignments[definition->assignment_count].value = value;
    ++definition->assignment_count;
    if (field == DK1_LEVEL_SPRITE_TYPE_FIELD) {
        definition->has_type_id = true;
        definition->type_id = value;
    }
    return true;
}

static bool definition_follow(const Dk1RomImage *rom, uint16_t address,
                              uint16_t seen[DK1_LEVEL_SPRITE_MAX_REDIRECTS + 1u],
                              size_t depth,
                              Dk1LevelSpriteDefinition *definition) {
    uint32_t pc;
    size_t i;
    if (depth > DK1_LEVEL_SPRITE_MAX_REDIRECTS) return false;
    for (i = 0u; i < depth; ++i) if (seen[i] == address) return false;
    seen[depth] = address;
    pc = bank_pc(DK1_LEVEL_SPRITE_DEFINITION_BANK, address);
    for (i = 0u; i < DK1_LEVEL_SPRITE_MAX_ASSIGNMENTS; ++i) {
        uint16_t word, argument;
        if (!dk1_rom_read_u16(rom, pc, &word) ||
            !dk1_rom_read_u16(rom, pc + 2u, &argument)) return false;
        if ((word & 0x8000u) == 0u) {
            if (!assignment_set(definition, word, argument)) return false;
            pc += 4u;
            continue;
        }
        definition->final_address = (uint16_t)pc;
        definition->terminal_word = word;
        definition->terminal_argument = argument;
        definition->terminal_command =
            (uint8_t)(((word >> 8u) - 0x80u) & 0xFFu);
        if (definition->terminal_command == 2u) {
            ++definition->redirect_count;
            return definition_follow(rom, argument, seen, depth + 1u,
                                     definition);
        }
        return true;
    }
    return false;
}

bool dk1_level_sprite_list_read(const Dk1RomImage *rom, uint16_t entrance_id,
                                Dk1LevelSpriteList *list) {
    uint16_t address;
    size_t i;
    if (rom == NULL || list == NULL ||
        !dk1_rom_read_u16(rom, DK1_LEVEL_SPRITE_POINTER_TABLE +
                          (uint32_t)entrance_id * 2u, &address) ||
        address < 0x8000u)
        return false;
    memset(list, 0, sizeof(*list));
    list->entrance_id = entrance_id;
    list->source_address = address;
    list->source_pc = bank_pc(DK1_LEVEL_SPRITE_DATA_BANK, address);
    for (i = 0u; i < DK1_LEVEL_SPRITE_MAX_RECORDS; ++i) {
        Dk1LevelSpriteRecord *record = &list->records[i];
        uint32_t pc = list->source_pc + (uint32_t)i * 8u;
        uint16_t y;
        if (!dk1_rom_read_u16(rom, pc, &record->command) ||
            !dk1_rom_read_u16(rom, pc + 2u, &record->world_x) ||
            !dk1_rom_read_u16(rom, pc + 4u, &y) ||
            !dk1_rom_read_u16(rom, pc + 6u, &record->definition_address))
            return false;
        if (record->command == 0u) {
            list->record_count = i;
            list->terminated = true;
            return true;
        }
        record->world_y = (int16_t)y;
        record->source_pc = pc;
    }
    list->record_count = DK1_LEVEL_SPRITE_MAX_RECORDS;
    return false;
}

bool dk1_level_sprite_definition_read(const Dk1RomImage *rom,
                                      uint16_t definition_address,
                                      Dk1LevelSpriteDefinition *definition) {
    uint16_t seen[DK1_LEVEL_SPRITE_MAX_REDIRECTS + 1u] = {0};
    if (rom == NULL || definition == NULL || definition_address < 0x8000u)
        return false;
    memset(definition, 0, sizeof(*definition));
    definition->source_address = definition_address;
    return definition_follow(rom, definition_address, seen, 0u, definition);
}

bool dk1_level_sprite_record_type(const Dk1RomImage *rom,
                                  const Dk1LevelSpriteRecord *record,
                                  uint16_t *type_id) {
    Dk1LevelSpriteDefinition definition;
    if (record == NULL || type_id == NULL ||
        !dk1_level_sprite_definition_read(rom, record->definition_address,
                                          &definition) ||
        !definition.has_type_id)
        return false;
    *type_id = definition.type_id;
    return true;
}

bool dk1_level_sprite_find_type(const Dk1RomImage *rom,
                                uint16_t entrance_id, uint16_t type_id,
                                size_t occurrence,
                                Dk1LevelSpriteRecord *record,
                                size_t *record_index) {
    Dk1LevelSpriteList list;
    size_t i, found = 0u;
    if (record == NULL || !dk1_level_sprite_list_read(rom, entrance_id, &list))
        return false;
    for (i = 0u; i < list.record_count; ++i) {
        uint16_t candidate;
        if (!dk1_level_sprite_record_type(rom, &list.records[i], &candidate) ||
            candidate != type_id)
            continue;
        if (found++ != occurrence) continue;
        *record = list.records[i];
        if (record_index != NULL) *record_index = i;
        return true;
    }
    return false;
}

static uint64_t hash_byte(uint64_t hash, uint8_t value) {
    hash ^= value;
    return hash * UINT64_C(1099511628211);
}

uint64_t dk1_level_sprite_list_signature(const Dk1LevelSpriteList *list) {
    uint64_t hash = UINT64_C(14695981039346656037);
    size_t i;
    if (list == NULL || !list->terminated) return 0u;
    for (i = 0u; i < list->record_count; ++i) {
        const Dk1LevelSpriteRecord *record = &list->records[i];
        const uint16_t words[4] = {
            record->command, record->world_x, (uint16_t)record->world_y,
            record->definition_address
        };
        size_t j;
        for (j = 0u; j < 4u; ++j) {
            hash = hash_byte(hash, (uint8_t)words[j]);
            hash = hash_byte(hash, (uint8_t)(words[j] >> 8u));
        }
    }
    return hash;
}
