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
        DK1_SCROLL_WRITE_ALL,
    };
    return state;
}

Dk1PpuScrollState dk1_ppu_scroll_quarter_vertical(
    uint16_t camera_x,
    uint16_t camera_y,
    uint16_t bg3_y
) {
    const Dk1PpuScrollState state = {
        camera_x,
        camera_y,
        (uint16_t)(camera_x >> 1u),
        (uint16_t)(camera_y >> 2u),
        0u,
        bg3_y,
        DK1_SCROLL_WRITE_BG1_X | DK1_SCROLL_WRITE_BG1_Y |
        DK1_SCROLL_WRITE_BG2_X | DK1_SCROLL_WRITE_BG2_Y |
        DK1_SCROLL_WRITE_BG3_Y,
    };
    return state;
}

Dk1PpuScrollState dk1_ppu_scroll_bg3_one_and_half(
    uint16_t camera_x,
    uint16_t camera_y
) {
    const Dk1PpuScrollState state = {
        camera_x,
        camera_y,
        (uint16_t)(camera_x >> 1u),
        0xFFFFu,
        (uint16_t)(camera_x + (camera_x >> 1u)),
        camera_y,
        DK1_SCROLL_WRITE_ALL,
    };
    return state;
}
