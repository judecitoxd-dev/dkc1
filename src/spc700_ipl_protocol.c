#include "dk1/spc700_ipl_protocol.h"
#include <string.h>

bool dk1_spc700_ipl_init(Dk1Spc700IplProtocol *p,
                         uint8_t *ram, size_t ram_size) {
    if (p == 0 || ram == 0 || ram_size < 65536u) return false;
    memset(p, 0, sizeof(*p));
    p->ram = ram;
    p->ram_size = ram_size;
    p->ports[0] = DK1_SPC_IPL_READY0;
    p->ports[1] = DK1_SPC_IPL_READY1;
    p->state = DK1_SPC_IPL_WAIT_BEGIN;
    return true;
}

bool dk1_spc700_ipl_begin(Dk1Spc700IplProtocol *p,
                          uint8_t command, uint16_t destination) {
    if (p == 0 || p->ram == 0 || p->state != DK1_SPC_IPL_WAIT_BEGIN ||
        command != DK1_SPC_IPL_BEGIN) return false;
    p->write_address = destination;
    p->ports[0] = command;
    p->ports[2] = (uint8_t)destination;
    p->ports[3] = (uint8_t)(destination >> 8u);
    p->expected_token = 0u;
    p->state = DK1_SPC_IPL_TRANSFER;
    return true;
}

bool dk1_spc700_ipl_write_byte(Dk1Spc700IplProtocol *p,
                               uint8_t token, uint8_t value) {
    if (p == 0 || p->ram == 0 || p->state != DK1_SPC_IPL_TRANSFER ||
        token != p->expected_token || p->write_address >= p->ram_size)
        return false;
    p->ports[0] = token;
    p->ports[1] = value;
    p->ram[p->write_address++] = value;
    p->bytes_written++;
    p->expected_token++;
    return true;
}

bool dk1_spc700_ipl_launch(Dk1Spc700IplProtocol *p,
                           uint8_t token, uint16_t entry_point) {
    if (p == 0 || p->ram == 0 || p->state != DK1_SPC_IPL_TRANSFER ||
        token == p->expected_token) return false;
    p->ports[0] = token;
    p->ports[2] = (uint8_t)entry_point;
    p->ports[3] = (uint8_t)(entry_point >> 8u);
    p->pc = entry_point;
    p->state = DK1_SPC_IPL_RUNNING;
    return true;
}
