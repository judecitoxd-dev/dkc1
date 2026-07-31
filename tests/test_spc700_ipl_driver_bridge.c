#include <assert.h>
#include <stdlib.h>
#include "dk1/rom_image.h"
#include "dk1/spc700_driver_startup.h"
#include "dk1/spc700_ipl_protocol.h"

int main(void) {
    const char *path = getenv("DK1_TEST_ROM");
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom;
    Dk1Spc700DriverStartup driver;
    Dk1Spc700DriverTrace trace;
    Dk1Spc700IplProtocol ipl;
    if (path == NULL) path = "/mnt/data/Donkey Kong Country (USA) (Rev 2).sfc";
    assert(dk1_rom_load_file(path, &owned, &rom));
    assert(dk1_spc700_driver_startup_load(&rom, &driver));
    assert(dk1_spc700_driver_trace_entry(&driver, 0x5Au, 64u, &trace));
    assert(trace.stop == DK1_SPC_TRACE_IPL_BRK_HANDOFF && trace.pc == 0xFFC0u);
    assert(dk1_spc700_ipl_init(&ipl, driver.ram, sizeof(driver.ram)));
    assert(dk1_spc700_ipl_begin(&ipl, DK1_SPC_IPL_BEGIN, 0x3000u));
    assert(dk1_spc700_ipl_write_byte(&ipl, 0u, 0x11u));
    assert(dk1_spc700_ipl_write_byte(&ipl, 1u, 0x22u));
    assert(dk1_spc700_ipl_write_byte(&ipl, 2u, 0x33u));
    assert(dk1_spc700_ipl_launch(&ipl, 4u, DK1_SPC_DRIVER_ENTRY));
    assert(ipl.pc == DK1_SPC_DRIVER_ENTRY && ipl.bytes_written == 3u);
    assert(driver.ram[0x3000u] == 0x11u && driver.ram[0x3002u] == 0x33u);
    assert(driver.ram[DK1_SPC_DRIVER_ENTRY] == 0x00u);
    dk1_rom_free(&owned);
    return 0;
}
