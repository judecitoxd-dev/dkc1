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

static bool linked_required(Dk1AnimationLinkedObject *linked,
                            Dk1AnimationScriptResult *result,
                            uint16_t processed) {
    if (linked != NULL) return false;
    result->status = DK1_ANIMATION_LINK_REQUIRED;
    result->commands_processed = processed;
    return true;
}

bool dk1_animation_script_pointer(const Dk1RomImage *rom, uint16_t animation_id,
                                  uint16_t *pointer) {
    uint32_t pc;
    if (rom == NULL || pointer == NULL) return false;
    pc = DK1_ANIMATION_POINTER_TABLE + (uint32_t)animation_id * 2u;
    return dk1_rom_read_u16(rom, pc, pointer) && *pointer >= 0x8000u;
}

static bool step_reverse(const Dk1RomImage *rom,
                         Dk1ObjectAnimationScriptState *s,
                         Dk1AnimationScriptResult *r) {
    uint16_t pointer = s->script_pointer;
    unsigned processed = 0u;
    int32_t accumulator = (int32_t)s->accumulator + (int32_t)s->speed;
    s->accumulator = (int16_t)accumulator;
    r->reverse = true;
    if (s->accumulator > 0) return true;

    while (processed < DK1_ANIMATION_MAX_COMMANDS_PER_TICK) {
        uint16_t record = (uint16_t)(pointer - 1u);
        uint16_t word;
        ++processed;
        if (!read16(rom, record, &word)) {
            r->status = DK1_ANIMATION_INVALID_DATA;
            r->commands_processed = (uint16_t)processed;
            return true;
        }
        if ((word & 0x8000u) != 0u) {
            if (!read16(rom, (uint16_t)(record + 2u), &pointer) || pointer < 0x8000u) {
                r->status = DK1_ANIMATION_INVALID_DATA;
                r->commands_processed = (uint16_t)processed;
                return true;
            }
            continue;
        }
        s->accumulator = (int16_t)((uint16_t)s->accumulator + (word & 0xFF00u));
        if (s->accumulator < 0) {
            pointer = (uint16_t)(pointer - 4u);
            continue;
        }
        if (!read16(rom, (uint16_t)(record + 2u), &s->frame)) {
            r->status = DK1_ANIMATION_INVALID_DATA;
            r->commands_processed = (uint16_t)processed;
            return true;
        }
        s->script_pointer = (uint16_t)(pointer - 3u);
        r->status = DK1_ANIMATION_FRAME_READY;
        r->frame_changed = true;
        r->commands_processed = (uint16_t)processed;
        if (s->callback_bank != 0u && s->callback_address != 0u)
            r->callback_pc = ((uint32_t)s->callback_bank << 16u) | s->callback_address;
        return true;
    }
    r->status = DK1_ANIMATION_INVALID_DATA;
    r->commands_processed = (uint16_t)processed;
    return true;
}

static bool timed_link_record(const Dk1RomImage *rom, uint16_t record,
                              Dk1ObjectAnimationScriptState *s,
                              Dk1AnimationLinkedObject *linked,
                              bool linked_frame, bool linked_motion,
                              uint16_t next_pointer,
                              Dk1AnimationScriptResult *r,
                              uint16_t processed) {
    uint8_t duration;
    uint16_t own_frame;
    if (!read8(rom, (uint16_t)(record + 2u), &duration)) return false;
    s->accumulator = (int16_t)((uint16_t)s->accumulator + ((uint16_t)duration << 8u));
    if (s->accumulator < 0) {
        s->script_pointer = next_pointer;
        return true;
    }
    if (!read16(rom, (uint16_t)(record + 3u), &own_frame)) return false;
    s->frame = own_frame;
    if (linked_frame) {
        if (!read16(rom, (uint16_t)(record + 5u), &linked->frame)) return false;
        linked->speed = 0;
        r->linked_frame_changed = true;
    }
    if (linked_motion) {
        uint16_t a, b;
        uint16_t base = (uint16_t)(record + (linked_frame ? 7u : 5u));
        if (!read16(rom, base, &a) || !read16(rom, (uint16_t)(base + 2u), &b)) return false;
        linked->field_13e9 = (int16_t)a;
        linked->field_145d = (int16_t)b;
        r->linked_motion_changed = true;
    }
    s->script_pointer = next_pointer;
    r->status = DK1_ANIMATION_FRAME_READY;
    r->frame_changed = true;
    r->commands_processed = processed;
    return true;
}

bool dk1_object_animation_step_with_links(const Dk1RomImage *rom,
                                          Dk1ObjectAnimationScriptState *s,
                                          const Dk1ObjectAnimationLinks *links,
                                          Dk1AnimationScriptResult *r) {
    uint16_t pointer;
    uint16_t word;
    unsigned processed = 0u;
    int32_t accumulator;
    if (rom == NULL || s == NULL || r == NULL || s->script_pointer < 0x8000u) return false;
    memset(r, 0, sizeof(*r));
    r->status = DK1_ANIMATION_IDLE;

    if (s->speed < 0) return step_reverse(rom, s, r);

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
                s->script_pointer = pointer;
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
        case 0x00u: /* $80 */
        case 0x11u: /* $91 */
            if (!dk1_animation_script_pointer(rom, s->animation_id, &pointer)) {
                r->status = DK1_ANIMATION_INVALID_DATA;
                r->commands_processed = (uint16_t)processed;
                return true;
            }
            s->script_pointer = pointer;
            continue;
        case 0x01u: { /* $81 */
            uint16_t address;
            uint8_t bank;
            if (!read16(rom, (uint16_t)(record + 2u), &address) ||
                !read8(rom, (uint16_t)(record + 4u), &bank)) {
                r->status = DK1_ANIMATION_INVALID_DATA;
                r->commands_processed = (uint16_t)processed;
                return true;
            }
            s->script_pointer = (uint16_t)(record + 5u);
            r->external_pc = ((uint32_t)bank << 16u) | address;
            r->status = DK1_ANIMATION_EXTERNAL_CALL;
            r->commands_processed = (uint16_t)processed;
            return true;
        }
        case 0x02u: /* $82 */
            if (!read16(rom, (uint16_t)(record + 2u), &pointer) || pointer < 0x8000u) {
                r->status = DK1_ANIMATION_INVALID_DATA;
                r->commands_processed = (uint16_t)processed;
                return true;
            }
            s->script_pointer = pointer;
            continue;
        case 0x03u: { /* $83 */
            uint16_t address;
            if (!read16(rom, (uint16_t)(record + 2u), &address)) {
                r->status = DK1_ANIMATION_INVALID_DATA;
                r->commands_processed = (uint16_t)processed;
                return true;
            }
            s->script_pointer = (uint16_t)(record + 1u);
            r->external_pc = be_pc(address);
            r->status = DK1_ANIMATION_EXTERNAL_CALL;
            r->commands_processed = (uint16_t)processed;
            return true;
        }
        case 0x04u: { /* $84 */
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
        case 0x05u: { /* $85: own + primary linked frame */
            Dk1AnimationLinkedObject *link = links == NULL ? NULL : links->primary;
            if (linked_required(link, r, (uint16_t)processed)) return true;
            if (!timed_link_record(rom, record, s, link, true, false,
                                   (uint16_t)(record + 7u), r, (uint16_t)processed))
                r->status = DK1_ANIMATION_INVALID_DATA;
            return true;
        }
        case 0x06u: { /* $86: own + primary frame and motion */
            Dk1AnimationLinkedObject *link = links == NULL ? NULL : links->primary;
            if (linked_required(link, r, (uint16_t)processed)) return true;
            if (!timed_link_record(rom, record, s, link, true, true,
                                   (uint16_t)(record + 11u), r, (uint16_t)processed))
                r->status = DK1_ANIMATION_INVALID_DATA;
            return true;
        }
        case 0x07u: { /* $87: own frame + primary motion */
            Dk1AnimationLinkedObject *link = links == NULL ? NULL : links->primary;
            if (linked_required(link, r, (uint16_t)processed)) return true;
            if (!timed_link_record(rom, record, s, link, false, true,
                                   (uint16_t)(record + 9u), r, (uint16_t)processed))
                r->status = DK1_ANIMATION_INVALID_DATA;
            return true;
        }
        case 0x08u: { /* $88: primary motion only */
            Dk1AnimationLinkedObject *link = links == NULL ? NULL : links->primary;
            uint16_t a, b;
            if (linked_required(link, r, (uint16_t)processed)) return true;
            if (!read16(rom, (uint16_t)(record + 2u), &a) ||
                !read16(rom, (uint16_t)(record + 4u), &b)) {
                r->status = DK1_ANIMATION_INVALID_DATA;
                r->commands_processed = (uint16_t)processed;
                return true;
            }
            link->field_13e9 = (int16_t)a;
            link->field_145d = (int16_t)b;
            r->linked_motion_changed = true;
            pointer = (uint16_t)(record + 6u);
            s->script_pointer = pointer;
            continue;
        }
        case 0x09u: { /* $89: own + secondary linked frame */
            Dk1AnimationLinkedObject *link = links == NULL ? NULL : links->secondary;
            if (linked_required(link, r, (uint16_t)processed)) return true;
            if (!timed_link_record(rom, record, s, link, true, false,
                                   (uint16_t)(record + 7u), r, (uint16_t)processed))
                r->status = DK1_ANIMATION_INVALID_DATA;
            return true;
        }
        case 0x0Au: { /* $8A: own + secondary frame and motion */
            Dk1AnimationLinkedObject *link = links == NULL ? NULL : links->secondary;
            if (linked_required(link, r, (uint16_t)processed)) return true;
            if (!timed_link_record(rom, record, s, link, true, true,
                                   (uint16_t)(record + 11u), r, (uint16_t)processed))
                r->status = DK1_ANIMATION_INVALID_DATA;
            return true;
        }
        case 0x0Bu: { /* $8B: own frame + secondary motion */
            Dk1AnimationLinkedObject *link = links == NULL ? NULL : links->secondary;
            if (linked_required(link, r, (uint16_t)processed)) return true;
            if (!timed_link_record(rom, record, s, link, false, true,
                                   (uint16_t)(record + 9u), r, (uint16_t)processed))
                r->status = DK1_ANIMATION_INVALID_DATA;
            return true;
        }
        case 0x0Cu: { /* $8C: secondary motion only */
            Dk1AnimationLinkedObject *link = links == NULL ? NULL : links->secondary;
            uint16_t a, b;
            if (linked_required(link, r, (uint16_t)processed)) return true;
            if (!read16(rom, (uint16_t)(record + 2u), &a) ||
                !read16(rom, (uint16_t)(record + 4u), &b)) {
                r->status = DK1_ANIMATION_INVALID_DATA;
                r->commands_processed = (uint16_t)processed;
                return true;
            }
            link->field_13e9 = (int16_t)a;
            link->field_145d = (int16_t)b;
            r->linked_motion_changed = true;
            pointer = (uint16_t)(record + 6u);
            s->script_pointer = pointer;
            continue;
        }
        case 0x0Du: { /* $8D: direct $1375-linked motion */
            Dk1AnimationLinkedObject *link = links == NULL ? NULL : links->direct;
            uint16_t a, b;
            if (link == NULL) {
                /* The original command is a no-op when $1375 is zero. */
                pointer = (uint16_t)(record + 6u);
                s->script_pointer = pointer;
                continue;
            }
            if (!read16(rom, (uint16_t)(record + 2u), &a) ||
                !read16(rom, (uint16_t)(record + 4u), &b)) {
                r->status = DK1_ANIMATION_INVALID_DATA;
                r->commands_processed = (uint16_t)processed;
                return true;
            }
            link->field_13e9 = (int16_t)a;
            link->field_145d = (int16_t)b;
            r->linked_motion_changed = true;
            pointer = (uint16_t)(record + 6u);
            s->script_pointer = pointer;
            continue;
        }
        case 0x0Eu:
        case 0x0Fu:
        case 0x10u: {
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

bool dk1_object_animation_step(const Dk1RomImage *rom,
                               Dk1ObjectAnimationScriptState *state,
                               Dk1AnimationScriptResult *result) {
    return dk1_object_animation_step_with_links(rom, state, NULL, result);
}
