#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "dk1/apu_driver_catalog.h"

int main(void) {
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom;
    Dk1ApuDriverSource source;
    Dk1ApuUploadBlock block;
    uint8_t *spc_ram;
    size_t valid = 0u, bytes = 0u;
    const char *path = getenv("DK1_TEST_ROM");
    if (path == NULL) path = "/mnt/data/Donkey Kong Country (USA) (Rev 2).sfc";
    assert(dk1_rom_load_file(path, &owned, &rom));
    assert(dk1_apu_driver_source_count() == 27u);
    assert(dk1_apu_driver_source_get(&rom, 0u, &source));
    assert(source.source_pc == 0x8A9361u);
    assert(dk1_apu_driver_source_get(&rom, 9u, &source));
    assert(source.source_pc == 0xD3D88Cu);
    assert(dk1_apu_driver_source_get(&rom, 10u, &source));
    assert(source.source_pc == 0xEA0000u);
    assert(dk1_apu_driver_source_get(&rom, 26u, &source));
    assert(source.source_pc == 0xD3E789u);
    assert(!dk1_apu_driver_source_get(&rom, 27u, &source));
    assert(dk1_apu_upload_block_get(&rom, 9u, &block));
    assert(block.length == 0x0A23u && block.target == 0x12A0u && block.data_pc == 0xD3D890u);
    spc_ram = (uint8_t *)malloc(DK1_SPC_RAM_SIZE);
    assert(spc_ram != NULL);
    {
        uint16_t index;
        for (index = 0u; index < DK1_APU_DRIVER_SOURCE_COUNT; ++index) {
            uint8_t first, last;
            memset(spc_ram, 0, DK1_SPC_RAM_SIZE);
            assert(dk1_apu_upload_block_get(&rom, index, &block));
            assert(dk1_rom_read_u8(&rom, block.data_pc, &first));
            assert(dk1_rom_read_u8(&rom, block.data_pc + block.length - 1u, &last));
            assert(dk1_apu_upload_block_load(&rom, &block, spc_ram));
            assert(spc_ram[block.target] == first);
            assert(spc_ram[(uint16_t)(block.target + block.length - 1u)] == last);
        }
    }
    free(spc_ram);
    assert(dk1_apu_driver_catalog_validate(&rom, &valid, &bytes));
    assert(valid == 27u && bytes == 65195u);
    assert(dk1_apu_driver_catalog_signature(&rom) == 0xD66EA2B33D4342BDull);
    assert(dk1_apu_driver_payload_signature(&rom) == 0x6CC614324FAED0E8ull);
    dk1_rom_free(&owned);
    return 0;
}
