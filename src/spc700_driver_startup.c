#include "dk1/spc700_driver_startup.h"
#include <string.h>

static uint64_t fnv1a(const uint8_t *data, size_t size) {
    uint64_t hash = 14695981039346656037ull;
    size_t i;
    for (i = 0u; i < size; ++i) {
        hash ^= data[i];
        hash *= 1099511628211ull;
    }
    return hash;
}

bool dk1_spc700_driver_startup_load(const Dk1RomImage *rom,
                                    Dk1Spc700DriverStartup *s) {
    uint16_t length, target;
    if (rom == NULL || s == NULL ||
        !dk1_rom_read_u16(rom, DK1_SPC_DRIVER_DATA_BLOCK, &length) ||
        !dk1_rom_read_u16(rom, DK1_SPC_DRIVER_DATA_BLOCK + 2u, &target) ||
        (uint32_t)target + length > DK1_SPC_RAM_SIZE) return false;
    memset(s, 0, sizeof(*s));
    if (!dk1_rom_copy(rom, DK1_SPC_DRIVER_CODE_SOURCE,
                      s->ram + DK1_SPC_DRIVER_CODE_TARGET,
                      DK1_SPC_DRIVER_CODE_SIZE) ||
        !dk1_rom_copy(rom, DK1_SPC_DRIVER_DATA_BLOCK + 4u,
                      s->ram + target, length)) return false;
    s->data_target = target;
    s->data_length = length;
    s->code_signature = fnv1a(s->ram + DK1_SPC_DRIVER_CODE_TARGET,
                              DK1_SPC_DRIVER_CODE_SIZE);
    s->data_signature = fnv1a(s->ram + target, length);
    s->ram_signature = fnv1a(s->ram, sizeof(s->ram));
    return true;
}

static void push8(Dk1Spc700DriverStartup *s, Dk1Spc700DriverTrace *t, uint8_t value) {
    s->ram[0x0100u | t->sp] = value;
    t->sp--;
}

bool dk1_spc700_driver_trace_entry(Dk1Spc700DriverStartup *s,
                                   uint8_t initial_port0,
                                   size_t limit,
                                   Dk1Spc700DriverTrace *t) {
    if (s == NULL || t == NULL || limit == 0u) return false;
    memset(t, 0, sizeof(*t));
    t->pc = DK1_SPC_DRIVER_ENTRY;
    t->sp = 0xEFu;
    s->ram[0x00F4u] = initial_port0;
    while (t->instructions < limit) {
        uint16_t pc = t->pc;
        uint8_t op = s->ram[pc];
        switch (op) {
        case 0x00u:
            t->pc = (uint16_t)(pc + 1u);
            break;
        case 0x20u:
            t->psw &= (uint8_t)~0x20u;
            t->pc = (uint16_t)(pc + 1u);
            break;
        case 0xCDu:
            t->x = s->ram[(uint16_t)(pc + 1u)];
            t->pc = (uint16_t)(pc + 2u);
            break;
        case 0xBDu:
            t->sp = t->x;
            t->pc = (uint16_t)(pc + 1u);
            break;
        case 0xE4u: {
            uint8_t dp = s->ram[(uint16_t)(pc + 1u)];
            t->a = s->ram[dp];
            t->pc = (uint16_t)(pc + 2u);
            break;
        }
        case 0xC4u: {
            uint8_t dp = s->ram[(uint16_t)(pc + 1u)];
            s->ram[dp] = t->a;
            t->pc = (uint16_t)(pc + 2u);
            break;
        }
        case 0xC5u: {
            uint16_t address = (uint16_t)(s->ram[(uint16_t)(pc + 1u)] |
                                ((uint16_t)s->ram[(uint16_t)(pc + 2u)] << 8u));
            s->ram[address] = t->a;
            t->pc = (uint16_t)(pc + 3u);
            break;
        }
        case 0x3Fu: {
            uint16_t target = (uint16_t)(s->ram[(uint16_t)(pc + 1u)] |
                              ((uint16_t)s->ram[(uint16_t)(pc + 2u)] << 8u));
            uint16_t return_pc = (uint16_t)(pc + 3u);
            push8(s, t, (uint8_t)(return_pc >> 8u));
            push8(s, t, (uint8_t)return_pc);
            t->pc = target;
            break;
        }
        default:
            t->unsupported_pc = pc;
            t->unsupported_opcode = op;
            t->stop = DK1_SPC_TRACE_UNSUPPORTED_OPCODE;
            return true;
        }
        t->instructions++;
    }
    t->stop = DK1_SPC_TRACE_LIMIT;
    return true;
}
