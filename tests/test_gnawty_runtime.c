#include <assert.h>
#include <stdlib.h>
#include "dk1/gnawty_runtime.h"

int main(void) {
    const char *path = getenv("DK1_TEST_ROM");
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom;
    Dk1GnawtyRuntime gnawty;
    Dk1GnawtyStepResult result;
    Dk1PlayerPreviewRuntime player;
    Dk1VramImage base_vram = {0};
    uint16_t start_x;
    unsigned i;

    if (path == NULL)
        path = "/mnt/data/Donkey Kong Country (USA) (Rev 2).sfc";
    assert(dk1_rom_load_file(path, &owned, &rom));
    assert(dk1_gnawty_spawn(&gnawty, &rom, 4u, 200u, 100, false));
    assert(gnawty.active);
    assert(gnawty.type_id == DK1_OBJECT_TYPE_GNAWTY);
    assert(gnawty.scheduler.type_id[4u] == DK1_OBJECT_TYPE_GNAWTY);
    assert(gnawty.animation.animation_id == DK1_ANIMATION_GNAWTY_WALK);

    start_x = gnawty.motion.world_x;
    for (i = 0u; i < 32u && gnawty.animation.frame == 0u; ++i) {
        assert(dk1_gnawty_step(&gnawty, &rom, NULL, NULL, &result));
        assert(gnawty.callback_verified);
        assert(gnawty.callback_pc == DK1_GNAWTY_CALLBACK_PC);
    }
    assert(gnawty.animation.frame != 0u);
    assert(gnawty.motion.world_x > start_x);
    assert(dk1_gnawty_build_visual(&gnawty, &rom, &base_vram,
                                    160u, 300u, 512u, 0u));
    assert(gnawty.visual_ready);
    assert(gnawty.visual.pieces > 0u);
    assert(gnawty.visual.dma_bytes > 0u);

    dk1_player_preview_init(&player, gnawty.motion.world_x,
                            (int16_t)(gnawty.motion.world_y + 14));
    player.motion.velocity_y = -0x0100;
    player.airborne = true;
    assert(dk1_gnawty_step(&gnawty, &rom, NULL, &player, &result));
    assert(result.stomped);
    assert(!result.player_hurt);
    assert(gnawty.phase == DK1_GNAWTY_DEFEATED);
    assert(gnawty.stomps == 1u);
    assert(player.motion.velocity_y == DK1_GNAWTY_STOMP_BOUNCE);
    assert(player.airborne);
    assert(gnawty.animation.animation_id == DK1_ANIMATION_GNAWTY_DEAD);

    for (i = 0u; i < DK1_GNAWTY_DEFEAT_FRAMES; ++i)
        assert(dk1_gnawty_step(&gnawty, &rom, NULL, NULL, &result));
    assert(!gnawty.active);
    assert(gnawty.scheduler.type_id[4u] == 0u);

    dk1_rom_free(&owned);
    return 0;
}
