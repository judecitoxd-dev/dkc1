#include <assert.h>
#include "dk1/ppu_scroll.h"

int main(void) {
    const Dk1PpuScrollState half = dk1_ppu_scroll_half_parallax(0x0123u, 0x00F1u);
    const Dk1PpuScrollState quarter = dk1_ppu_scroll_quarter_vertical(0x0100u, 0x0080u, 7u);
    const Dk1PpuScrollState deep = dk1_ppu_scroll_bg3_one_and_half(0x0100u, 0x0020u);
    assert(half.bg1_x == 0x0123u);
    assert(half.bg1_y == 0x00F1u);
    assert(half.bg2_x == 0x0091u);
    assert(half.bg2_y == 0x0078u);
    assert(half.write_mask == DK1_SCROLL_WRITE_ALL);
    assert(quarter.bg2_x == 0x0080u && quarter.bg2_y == 0x0020u);
    assert(quarter.bg3_y == 7u);
    assert((quarter.write_mask & DK1_SCROLL_WRITE_BG3_X) == 0u);
    assert(deep.bg2_y == 0xFFFFu);
    assert(deep.bg3_x == 0x0180u && deep.bg3_y == 0x0020u);
    return 0;
}
