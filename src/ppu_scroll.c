#include "dk1/ppu_scroll.h"

Dk1PpuScrollState dk1_ppu_scroll_half_parallax(
    uint16_t camera_x,
    uint16_t camera_y
) {
    const Dk1PpuScrollState state = {
        camera_x,
        camera_y,
        (uint16_t)(camera_x >> 1u),
        (uint16_t)(camera_y >> 1u),
        0u,
        0u,
    };
    return state;
}
