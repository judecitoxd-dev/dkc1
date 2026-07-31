#include <assert.h>
#include <string.h>
#include "dk1/player_visual_runtime.h"

int main(void) {
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom;
    Dk1VramImage base, output;
    Dk1OamImage oam;
    Dk1PlayerVisualStats stats;
    Dk1OamSprite first;
    Dk1ObjectScreenTransform transform = {100, 80, 20, 224, 0, 0, 0};
    Dk1ObjectFrameOamStyle style = {0x0040u, 0u, 3u, false, false};
    uint8_t expected;
    memset(&base, 0, sizeof(base));
    assert(dk1_rom_load_file("/mnt/data/Donkey Kong Country (USA) (Rev 2).sfc", &owned, &rom));
    assert(dk1_player_visual_build(&rom, &base, 0x0330u, transform, style, 0u,
                                   &oam, &output, &stats));
    assert(stats.pieces == 12u);
    assert(stats.dma_records == 2u && stats.dma_bytes == 576u);
    assert(stats.layout_signature == 0x9A2E475D9D1AB40Full);
    assert(dk1_oam_decode_sprite(&oam, 0u, 0u, &first));
    assert(dk1_rom_read_u8(&rom, 0xD7622Au, &expected));
    assert(output.bytes[0x0800u] == expected);
    dk1_rom_free(&owned);
    return 0;
}
