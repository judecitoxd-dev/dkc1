#include <assert.h>
#include "dk1/ppu_scroll.h"

int main(void) {
    const Dk1PpuScrollState state =
        dk1_ppu_scroll_half_parallax(0x0123u, 0x00F1u);

    assert(state.bg1_x == 0x0123u);
    assert(state.bg1_y == 0x00F1u);
    assert(state.bg2_x == 0x0091u);
    assert(state.bg2_y == 0x0078u);
    assert(state.bg3_x == 0u);
    assert(state.bg3_y == 0u);
    return 0;
}
