#include <stdio.h>
#include <string.h>
#include "dk1/apu_boot_image.h"
#include "dk1/object_animation_script.h"
#include "dk1/player_coverage.h"
#include "dk1/player_dispatch.h"
#include "dk1/rom_image.h"
#include "dk1/spc700_bootstrap.h"

int main(int argc, char **argv) {
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom = {0};
    Dk1RomIdentity identity;
    Dk1ApuBootImage apu;
    Dk1Spc700BootstrapCpu spc;
    Dk1ObjectAnimationScriptState animation;
    Dk1AnimationScriptResult animation_result;
    uint16_t state;
    unsigned invalid = 0u;
    bool spc_ok;

    if (argc != 2) {
        fprintf(stderr, "usage: %s ROM\n", argv[0]);
        return 2;
    }
    if (!dk1_rom_load_file(argv[1], &owned, &rom) ||
        !dk1_rom_identity(&rom, &identity) ||
        !dk1_rom_identity_is_supported_rev2(&identity) ||
        !dk1_apu_boot_image_read(&rom, &apu)) {
        fputs("unsupported ROM\n", stderr);
        dk1_rom_free(&owned);
        return 1;
    }

    for (state = 0u; state < DK1_PLAYER_STATE_COUNT; ++state) {
        uint8_t opcode;
        if (dk1_player_state_translation_level(state) != DK1_PLAYER_STATE_UNTRANSLATED &&
            !dk1_rom_read_u8(&rom, dk1_player_state_handler_pc(state), &opcode))
            ++invalid;
    }

    memset(&animation, 0, sizeof(animation));
    animation.animation_id = 0u;
    animation.speed = 0x0100;
    if (!dk1_animation_script_pointer(&rom, animation.animation_id, &animation.script_pointer) ||
        !dk1_object_animation_step(&rom, &animation, &animation_result) ||
        animation_result.status != DK1_ANIMATION_FRAME_READY)
        ++invalid;

    dk1_spc700_bootstrap_init(&spc, &apu);
    spc_ok = dk1_spc700_bootstrap_send_byte(&spc, 1u, 0x2000u, 0x12u, 256u) &&
             dk1_spc700_bootstrap_send_byte(&spc, 3u, 0x2001u, 0x34u, 256u) &&
             dk1_spc700_bootstrap_launch(&spc, 4u, 0x2000u, 256u);
    if (!spc_ok) ++invalid;

    printf("player_states=%u planned=%u local=%u invalid=%u "
           "translation=%016llX apu_boot=%016llX animation=%04X spc_steps=%llu\n",
           (unsigned)DK1_PLAYER_STATE_COUNT,
           (unsigned)dk1_player_planned_state_count(),
           (unsigned)dk1_player_translated_state_count(),
           invalid,
           (unsigned long long)dk1_player_translation_signature(),
           (unsigned long long)apu.signature,
           animation.frame,
           (unsigned long long)spc.instructions);
    dk1_rom_free(&owned);
    return invalid != 0u ? 1 : 0;
}
