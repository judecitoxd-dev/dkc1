#include <assert.h>
#include <string.h>
#include "dk1/spc700_ipl_protocol.h"

int main(void) {
    uint8_t ram[65536];
    Dk1Spc700IplProtocol p;
    memset(ram, 0, sizeof(ram));
    assert(dk1_spc700_ipl_init(&p, ram, sizeof(ram)));
    assert(p.ports[0] == 0xAAu && p.ports[1] == 0xBBu);
    assert(!dk1_spc700_ipl_begin(&p, 0xCBu, 0x3000u));
    assert(dk1_spc700_ipl_begin(&p, 0xCCu, 0x3000u));
    assert(dk1_spc700_ipl_write_byte(&p, 0u, 0x11u));
    assert(!dk1_spc700_ipl_write_byte(&p, 2u, 0x22u));
    assert(dk1_spc700_ipl_write_byte(&p, 1u, 0x22u));
    assert(dk1_spc700_ipl_write_byte(&p, 2u, 0x33u));
    assert(ram[0x3000u] == 0x11u && ram[0x3002u] == 0x33u);
    assert(!dk1_spc700_ipl_launch(&p, 3u, 0x05E8u));
    assert(dk1_spc700_ipl_launch(&p, 4u, 0x05E8u));
    assert(p.state == DK1_SPC_IPL_RUNNING && p.pc == 0x05E8u);
    assert(p.bytes_written == 3u);
    return 0;
}
