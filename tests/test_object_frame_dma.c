#include <assert.h>
#include <stdlib.h>
#include "dk1/object_frame_dma.h"

int main(void) {
    const char *path = getenv("DK1_TEST_ROM");
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom;
    Dk1ObjectFrameLayout layout;
    Dk1NmiVramDmaQueue queue;
    Dk1ObjectFrameDmaStats stats;
    if (path == NULL) path = "/mnt/data/Donkey Kong Country (USA) (Rev 2).sfc";
    assert(dk1_rom_load_file(path, &owned, &rom));
    assert(dk1_object_frame_layout_read(&rom, 0x0330u, &layout));
    dk1_nmi_vram_dma_init(&queue);
    assert(dk1_object_frame_dma_schedule(&layout, 0x0040u, &queue, &stats));
    assert(stats.records == 2u && stats.bytes == 576u && stats.graphics_source_pc == 0xD7622Au);
    assert(queue.records[0].length == 448u);
    assert(queue.records[0].vram_word_address == 0x0400u);
    assert(queue.records[0].source_address == 0x622Au);
    assert(queue.records[0].source_bank_active == 0x80D7u);
    assert(queue.records[1].length == 128u);
    assert(queue.records[1].vram_word_address == 0x0500u);
    assert(queue.records[1].source_address == 0x63EAu);
    assert(queue.records[1].source_bank_active == 0x80D7u);
    assert(queue.records[2].source_bank_active == 0u);
    dk1_rom_free(&owned);
    return 0;
}
