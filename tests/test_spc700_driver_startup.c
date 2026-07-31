#include <assert.h>
#include <stdlib.h>
#include "dk1/spc700_driver_startup.h"

int main(void) {
    const char *path = getenv("DK1_TEST_ROM");
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom;
    Dk1Spc700DriverStartup startup;
    Dk1Spc700DriverTrace trace;
    if (path == NULL) path = "/mnt/data/Donkey Kong Country (USA) (Rev 2).sfc";
    assert(dk1_rom_load_file(path, &owned, &rom));
    assert(dk1_spc700_driver_startup_load(&rom, &startup));
    assert(startup.data_target == 0x2380u && startup.data_length == 0x0E26u);
    assert(startup.code_signature == 0xD046E1817CE18D5Cull);
    assert(startup.data_signature == 0x4D8B5E57A7B1359Aull);
    assert(startup.ram_signature == 0x856CF336ECC55C5Bull);
    assert(startup.ram[0x05E8u] == 0x00u && startup.ram[0x05ECu] == 0x20u);
    assert(startup.ram[0x2380u] == 0xB4u && startup.ram[0x2381u] == 0x2Fu);
    assert(dk1_spc700_driver_trace_entry(&startup, 0x5Au, 64u, &trace));
    assert(trace.instructions == 13u);
    assert(trace.stop == DK1_SPC_TRACE_IPL_BRK_HANDOFF);
    assert(trace.pc == 0xFFC0u && trace.brk_vector == 0xFFC0u);
    assert(trace.a == 0xF4u && trace.x == 0xFFu && trace.sp == 0xFAu);
    assert(startup.ram[0x04DEu] == 0x5Au && startup.ram[0x00F4u] == 0x5Au);
    assert(startup.ram[0x01FFu] == 0x05u && startup.ram[0x01FEu] == 0xFAu);
    assert(startup.ram[0x01FDu] == 0x10u && startup.ram[0x01FCu] == 0x78u);
    dk1_rom_free(&owned);
    return 0;
}
