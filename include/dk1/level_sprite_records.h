#ifndef DK1_LEVEL_SPRITE_RECORDS_H
#define DK1_LEVEL_SPRITE_RECORDS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/rom_image.h"

#define DK1_LEVEL_SPRITE_POINTER_TABLE 0xBD8000u
#define DK1_LEVEL_SPRITE_DATA_BANK 0xBDu
#define DK1_LEVEL_SPRITE_DEFINITION_BANK 0xB5u
#define DK1_LEVEL_SPRITE_MAX_RECORDS 256u
#define DK1_LEVEL_SPRITE_MAX_ASSIGNMENTS 64u
#define DK1_LEVEL_SPRITE_MAX_REDIRECTS 16u
#define DK1_LEVEL_SPRITE_TYPE_FIELD 0x0D45u
#define DK1_LEVEL_SPRITE_COMMAND_NORMAL 0x0001u

typedef struct Dk1LevelSpriteRecord {
    uint16_t command;
    uint16_t world_x;
    int16_t world_y;
    uint16_t definition_address;
    uint32_t source_pc;
} Dk1LevelSpriteRecord;

typedef struct Dk1LevelSpriteList {
    uint16_t entrance_id;
    uint16_t source_address;
    uint32_t source_pc;
    size_t record_count;
    bool terminated;
    Dk1LevelSpriteRecord records[DK1_LEVEL_SPRITE_MAX_RECORDS];
} Dk1LevelSpriteList;

typedef struct Dk1LevelSpriteAssignment {
    uint16_t field_address;
    uint16_t value;
} Dk1LevelSpriteAssignment;

typedef struct Dk1LevelSpriteDefinition {
    uint16_t source_address;
    uint16_t final_address;
    uint16_t terminal_word;
    uint16_t terminal_argument;
    uint8_t terminal_command;
    uint8_t redirect_count;
    size_t assignment_count;
    bool has_type_id;
    uint16_t type_id;
    Dk1LevelSpriteAssignment assignments[DK1_LEVEL_SPRITE_MAX_ASSIGNMENTS];
} Dk1LevelSpriteDefinition;

bool dk1_level_sprite_list_read(const Dk1RomImage *rom, uint16_t entrance_id,
                                Dk1LevelSpriteList *list);
bool dk1_level_sprite_definition_read(const Dk1RomImage *rom,
                                      uint16_t definition_address,
                                      Dk1LevelSpriteDefinition *definition);
bool dk1_level_sprite_record_type(const Dk1RomImage *rom,
                                  const Dk1LevelSpriteRecord *record,
                                  uint16_t *type_id);
bool dk1_level_sprite_find_type(const Dk1RomImage *rom,
                                uint16_t entrance_id, uint16_t type_id,
                                size_t occurrence,
                                Dk1LevelSpriteRecord *record,
                                size_t *record_index);
uint64_t dk1_level_sprite_list_signature(const Dk1LevelSpriteList *list);

#endif
