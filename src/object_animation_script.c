#include "dk1/object_animation_script.h"
#include <string.h>

static uint32_t be_pc(uint16_t address) {
    return ((uint32_t)DK1_ANIMATION_SCRIPT_BANK << 16u) | address;
}

static bool read8(const Dk1RomImage *rom, uint16_t address, uint8_t *value) {
    return dk1_rom_read_u8(rom, be_pc(address), value);
}

static bool read16(const Dk1RomImage *rom, uint16_t address, uint16_t *value) {
    return dk1_rom_read_u16(rom, be_pc(address), value);
}

bool dk1_animation_script_pointer(const Dk1RomImage *rom, uint16_t animation_id,
                                  uint16_t *pointer) {
    uint32_t pc;
    if (rom == NULL || pointer == NULL) return false;
    pc = DK1_ANIMATION_POINTER_TABLE + (uint32_t)animation_id * 2u;
    return dk1_rom_read_u16(rom, pc, pointer) && *pointer >= 0x8000u;
}

bool dk1_object_animation_step(const Dk1RomImage *rom,
                               Dk1ObjectAnimationScriptState *s,
                               Dk1AnimationScriptResult *r) {
    uint16_t pointer;
    uint16_t word;
    unsigned processed = 0u;
    int32_t accumulator;
    if (rom == NULL || s == NULL || r == NULL || s->script_pointer < 0x8000u) return false;
    memset(r, 0, sizeof(*r));
    r->status = DK1_ANIMATION_IDLE;

    accumulator = (int32_t)s->accumulator - (int32_t)s->speed;
    s->accumulator = (int16_t)accumulator;
    if (s->accumulator > 0) {
        if (s->callback_bank != 0u && s->callback_address != 0u)
            r->callback_pc = ((uint32_t)s->callback_bank << 16u) | s->callback_address;
        return true;
    }

    pointer = s->script_pointer;
    while (processed < DK1_ANIMATION_MAX_COMMANDS_PER_TICK) {
        uint16_t record = (uint16_t)(pointer - 1u);
        uint8_t command;
        ++processed;
        if (!read16(rom, record, &word)) {
            r->status = DK1_ANIMATION_INVALID_DATA;
            r->commands_processed = (uint16_t)processed;
            return true;
        }
        if ((word & 0x8000u) == 0u) {
            s->accumulator = (int16_t)((uint16_t)s->accumulator + (word & 0xFF00u));
            if (s->accumulator < 0) {
                pointer = (uint16_t)(pointer + 4u);
                continue;
            }
            if (!read16(rom, (uint16_t)(record + 2u), &s->frame)) {
                r->status = DK1_ANIMATION_INVALID_DATA;
                r->commands_processed = (uint16_t)processed;
                return true;
            }
            pointer = (uint16_t)(record + 4u);
            s->script_pointer = pointer;
            r->status = DK1_ANIMATION_FRAME_READY;
            r->frame_changed = true;
            r->commands_processed = (uint16_t)processed;
            if (s->callback_bank != 0u && s->callback_address != 0u)
                r->callback_pc = ((uint32_t)s->callback_bank << 16u) | s->callback_address;
            return true;
        }

        command = (uint8_t)((word >> 8u) - 0x80u);
        r->command = (uint8_t)(command + 0x80u);
        switch (command) {
        case 0x00u: /* $80, $BE:814C */
        case 0x11u: /* $91 aliases $80 in the dispatch table */
            if (!dk1_animation_script_pointer(rom, s->animation_id, &pointer)) {
                r->status = DK1_ANIMATION_INVALID_DATA;
                r->commands_processed = (uint16_t)processed;
                return true;
            }
            s->script_pointer = pointer;
            continue;
        case 0x01u: { /* $81, long external call */
            uint16_t address;
            uint8_t bank;
            if (!read16(rom, (uint16_t)(record + 2u), &address) ||
                !read8(rom, (uint16_t)(record + 4u), &bank)) {
                r->status = DK1_ANIMATION_INVALID_DATA;
                r->commands_processed = (uint16_t)processed;
                return true;
            }
            s->script_pointer = (uint16_t)(record + 6u);
            r->external_pc = ((uint32_t)bank << 16u) | address;
            r->status = DK1_ANIMATION_EXTERNAL_CALL;
            r->commands_processed = (uint16_t)processed;
            return true;
        }
        case 0x02u: /* $82, jump to a 16-bit bank-$BE pointer */
            if (!read16(rom, (uint16_t)(record + 2u), &pointer) || pointer < 0x8000u) {
                r->status = DK1_ANIMATION_INVALID_DATA;
                r->commands_processed = (uint16_t)processed;
                return true;
            }
            s->script_pointer = pointer;
            continue;
        case 0x03u: { /* $83, local external call */
            uint16_t address;
            if (!read16(rom, (uint16_t)(record + 2u), &address)) {
                r->status = DK1_ANIMATION_INVALID_DATA;
                r->commands_processed = (uint16_t)processed;
                return true;
            }
            s->script_pointer = (uint16_t)(record + 2u);
            r->external_pc = be_pc(address);
            r->status = DK1_ANIMATION_EXTERNAL_CALL;
            r->commands_processed = (uint16_t)processed;
            return true;
        }
        case 0x04u: { /* $84, set $1341/$130D */
            uint16_t address;
            uint8_t bank;
            if (!read16(rom, (uint16_t)(record + 2u), &address) ||
                !read8(rom, (uint16_t)(record + 4u), &bank)) {
                r->status = DK1_ANIMATION_INVALID_DATA;
                r->commands_processed = (uint16_t)processed;
                return true;
            }
            s->callback_address = address;
            s->callback_bank = bank;
            r->callback_changed = true;
            pointer = (uint16_t)(record + 5u);
            s->script_pointer = pointer;
            continue;
        }
        case 0x0Eu: /* $8E -> $BF:FB5E */
        case 0x0Fu: /* $8F -> $BF:FB71 */
        case 0x10u: { /* $90 -> $BF:FB8F */
            uint8_t value;
            if (!read8(rom, (uint16_t)(record + 2u), &value)) {
                r->status = DK1_ANIMATION_INVALID_DATA;
                r->commands_processed = (uint16_t)processed;
                return true;
            }
            r->event_kind = (uint8_t)(command - 0x0Du);
            r->event_value = value;
            pointer = (uint16_t)(record + 3u);
            s->script_pointer = pointer;
            continue;
        }
        default:
            s->script_pointer = pointer;
            r->status = DK1_ANIMATION_UNSUPPORTED_COMMAND;
            r->commands_processed = (uint16_t)processed;
            return true;
        }
    }

    r->status = DK1_ANIMATION_INVALID_DATA;
    r->commands_processed = (uint16_t)processed;
    return true;
}
