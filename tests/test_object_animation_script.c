#include <assert.h>
#include <string.h>
#include "dk1/object_animation_script.h"

static void put16(uint8_t *rom, size_t offset, uint16_t value) {
    rom[offset] = (uint8_t)value;
    rom[offset + 1u] = (uint8_t)(value >> 8u);
}

int main(void) {
    uint8_t bytes[0x400000];
    Dk1RomImage rom;
    Dk1ObjectAnimationScriptState state;
    Dk1AnimationScriptResult result;
    Dk1AnimationLinkedObject primary, secondary, direct;
    Dk1ObjectAnimationLinks links;
    size_t bank_be = (size_t)(0xBEu & 0x3Fu) << 16u;
    memset(bytes, 0, sizeof(bytes));
    assert(dk1_rom_image_init(&rom, bytes, sizeof(bytes)));

    put16(bytes, bank_be + 0x8572u + 4u, 0x9001u);
    put16(bytes, bank_be + 0x9000u, 0x0200u);
    put16(bytes, bank_be + 0x9002u, 0x1234u);
    memset(&state, 0, sizeof(state));
    state.animation_id = 2u;
    state.script_pointer = 0x9001u;
    state.speed = 0x0100;
    assert(dk1_object_animation_step(&rom, &state, &result));
    assert(result.status == DK1_ANIMATION_FRAME_READY);
    assert(state.frame == 0x1234u && state.script_pointer == 0x9004u);

    bytes[bank_be + 0x9100u] = 0u;
    bytes[bank_be + 0x9101u] = 0x82u;
    put16(bytes, bank_be + 0x9102u, 0x9201u);
    put16(bytes, bank_be + 0x9200u, 0x0100u);
    put16(bytes, bank_be + 0x9202u, 0x4567u);
    memset(&state, 0, sizeof(state));
    state.script_pointer = 0x9101u;
    assert(dk1_object_animation_step(&rom, &state, &result));
    assert(result.status == DK1_ANIMATION_FRAME_READY && state.frame == 0x4567u);

    bytes[bank_be + 0x9300u] = 0u;
    bytes[bank_be + 0x9301u] = 0x84u;
    put16(bytes, bank_be + 0x9302u, 0xBEEFu);
    bytes[bank_be + 0x9304u] = 0xBFu;
    bytes[bank_be + 0x9305u] = 0x01u;
    put16(bytes, bank_be + 0x9306u, 0x7777u);
    memset(&state, 0, sizeof(state));
    state.script_pointer = 0x9301u;
    assert(dk1_object_animation_step(&rom, &state, &result));
    assert(result.status == DK1_ANIMATION_FRAME_READY);
    assert(result.callback_changed && result.callback_pc == 0xBFBEEFu);

    /* $85 requires the resolved $0512 link and writes both frames. */
    bytes[bank_be + 0x9400u] = 0u;
    bytes[bank_be + 0x9401u] = 0x85u;
    bytes[bank_be + 0x9402u] = 1u;
    put16(bytes, bank_be + 0x9403u, 0x1111u);
    put16(bytes, bank_be + 0x9405u, 0x2222u);
    memset(&state, 0, sizeof(state));
    state.script_pointer = 0x9401u;
    assert(dk1_object_animation_step(&rom, &state, &result));
    assert(result.status == DK1_ANIMATION_LINK_REQUIRED);
    memset(&primary, 0, sizeof(primary));
    links.primary = &primary; links.secondary = NULL; links.direct = NULL;
    memset(&state, 0, sizeof(state)); state.script_pointer = 0x9401u;
    assert(dk1_object_animation_step_with_links(&rom, &state, &links, &result));
    assert(result.status == DK1_ANIMATION_FRAME_READY);
    assert(state.frame == 0x1111u && primary.frame == 0x2222u && primary.speed == 0);

    /* $8A writes a secondary frame and two motion words. */
    bytes[bank_be + 0x9500u] = 0u;
    bytes[bank_be + 0x9501u] = 0x8Au;
    bytes[bank_be + 0x9502u] = 1u;
    put16(bytes, bank_be + 0x9503u, 0x3333u);
    put16(bytes, bank_be + 0x9505u, 0x4444u);
    put16(bytes, bank_be + 0x9507u, 0xFFF0u);
    put16(bytes, bank_be + 0x9509u, 0x0020u);
    memset(&secondary, 0, sizeof(secondary)); links.secondary = &secondary;
    memset(&state, 0, sizeof(state)); state.script_pointer = 0x9501u;
    assert(dk1_object_animation_step_with_links(&rom, &state, &links, &result));
    assert(state.frame == 0x3333u && secondary.frame == 0x4444u);
    assert(secondary.field_13e9 == -16 && secondary.field_145d == 32);

    /* $8D is a no-op without $1375 and writes the direct link when present. */
    bytes[bank_be + 0x9600u] = 0u;
    bytes[bank_be + 0x9601u] = 0x8Du;
    put16(bytes, bank_be + 0x9602u, 0x0012u);
    put16(bytes, bank_be + 0x9604u, 0xFFE0u);
    bytes[bank_be + 0x9606u] = 1u;
    put16(bytes, bank_be + 0x9607u, 0x5555u);
    memset(&state, 0, sizeof(state)); state.script_pointer = 0x9601u;
    links.direct = NULL;
    assert(dk1_object_animation_step_with_links(&rom, &state, &links, &result));
    assert(result.status == DK1_ANIMATION_FRAME_READY && state.frame == 0x5555u);
    memset(&direct, 0, sizeof(direct)); links.direct = &direct;
    memset(&state, 0, sizeof(state)); state.script_pointer = 0x9601u;
    assert(dk1_object_animation_step_with_links(&rom, &state, &links, &result));
    assert(direct.field_13e9 == 0x12 && direct.field_145d == -32);

    /* Negative speed follows the reverse-frame path at $BE:8414. */
    put16(bytes, bank_be + 0x9700u, 0x0100u);
    put16(bytes, bank_be + 0x9702u, 0x6666u);
    put16(bytes, bank_be + 0x9704u, 0x0100u);
    put16(bytes, bank_be + 0x9706u, 0x7777u);
    memset(&state, 0, sizeof(state));
    state.script_pointer = 0x9705u;
    state.speed = -0x0100;
    assert(dk1_object_animation_step(&rom, &state, &result));
    assert(result.reverse && result.status == DK1_ANIMATION_FRAME_READY);
    assert(state.frame == 0x7777u && state.script_pointer == 0x9702u);
    return 0;
}
