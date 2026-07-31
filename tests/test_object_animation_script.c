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
    size_t bank_be = (size_t)(0xBEu & 0x3Fu) << 16u;
    memset(bytes, 0, sizeof(bytes));
    assert(dk1_rom_image_init(&rom, bytes, sizeof(bytes)));

    /* Animation table id 2 -> $9001. */
    put16(bytes, bank_be + 0x8572u + 4u, 0x9001u);
    /* Normal record at pointer-1: duration $0200 and frame $1234. */
    put16(bytes, bank_be + 0x9000u, 0x0200u);
    put16(bytes, bank_be + 0x9002u, 0x1234u);
    memset(&state, 0, sizeof(state));
    state.animation_id = 2u;
    state.script_pointer = 0x9001u;
    state.speed = 0x0100;
    assert(dk1_object_animation_step(&rom, &state, &result));
    assert(result.status == DK1_ANIMATION_FRAME_READY);
    assert(state.frame == 0x1234u && state.script_pointer == 0x9004u);

    /* $82 jump command packed as [low,$82,target lo,target hi]. */
    bytes[bank_be + 0x9100u] = 0u;
    bytes[bank_be + 0x9101u] = 0x82u;
    put16(bytes, bank_be + 0x9102u, 0x9201u);
    put16(bytes, bank_be + 0x9200u, 0x0100u);
    put16(bytes, bank_be + 0x9202u, 0x4567u);
    memset(&state, 0, sizeof(state));
    state.script_pointer = 0x9101u;
    assert(dk1_object_animation_step(&rom, &state, &result));
    assert(result.status == DK1_ANIMATION_FRAME_READY && state.frame == 0x4567u);

    /* $84 installs a callback, then a packed normal record follows at record+4. */
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

    /* A complex paired-object command remains explicitly unsupported. */
    bytes[bank_be + 0x9400u] = 0u;
    bytes[bank_be + 0x9401u] = 0x85u;
    memset(&state, 0, sizeof(state));
    state.script_pointer = 0x9401u;
    assert(dk1_object_animation_step(&rom, &state, &result));
    assert(result.status == DK1_ANIMATION_UNSUPPORTED_COMMAND && result.command == 0x85u);
    return 0;
}
