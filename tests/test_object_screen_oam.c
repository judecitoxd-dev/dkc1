#include <assert.h>
#include <stdlib.h>
#include "dk1/object_screen_oam.h"

int main(void) {
    const char *path = getenv("DK1_TEST_ROM");
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom;
    Dk1ObjectFrameLayout layout;
    Dk1ObjectFrameOamStyle style = {0x40u, 2u, 3u, false, false};
    Dk1ObjectScreenTransform t = {100, 80, 20, 224, 0, 0, 0};
    Dk1ObjectScreenOamStats stats;
    Dk1OamImage oam;
    Dk1OamSprite first, second;
    if (path == NULL) path = "/mnt/data/Donkey Kong Country (USA) (Rev 2).sfc";
    assert(dk1_rom_load_file(path, &owned, &rom));
    assert(dk1_object_frame_layout_read(&rom, 0x0330u, &layout));
    dk1_oam_init(&oam);
    assert(dk1_object_frame_emit_screen_oam(&layout, t, style, 0u, 0u, &oam, &stats));
    assert(stats.considered == 12u && stats.written == 12u && stats.vertically_clipped == 0u);
    assert(dk1_oam_decode_sprite(&oam, 0u, 0u, &first));
    assert(dk1_oam_decode_sprite(&oam, 0u, 1u, &second));
    assert(first.x == 78 && first.y == 116u && first.width == 16u && first.height == 16u);
    assert(second.x == 72 && second.y == 132u);
    assert(first.tile_number == 0x40u && second.tile_number == 0x42u);

    dk1_oam_init(&oam);
    t.object_flags = 0x4000u;
    assert(dk1_object_frame_emit_screen_oam(&layout, t, style, 0u, 0u, &oam, &stats));
    assert(dk1_oam_decode_sprite(&oam, 0u, 0u, &first));
    assert(first.x == 66 && first.y == 116u && first.horizontal_flip);

    dk1_oam_init(&oam);
    t.object_flags = 0x8000u;
    assert(dk1_object_frame_emit_screen_oam(&layout, t, style, 0u, 0u, &oam, &stats));
    assert(dk1_oam_decode_sprite(&oam, 0u, 0u, &first));
    assert(first.x == 78 && first.y == 156u && first.vertical_flip);

    t.world_y = -1000;
    dk1_oam_init(&oam);
    assert(dk1_object_frame_emit_screen_oam(&layout, t, style, 0u, 0u, &oam, &stats));
    assert(stats.written == 0u && stats.vertically_clipped == 12u);
    dk1_rom_free(&owned);
    return 0;
}
