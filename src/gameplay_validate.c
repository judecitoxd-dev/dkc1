#include <stdio.h>
#include <string.h>
#include "dk1/apu_boot_image.h"
#include "dk1/apu_driver_catalog.h"
#include "dk1/nmi_vram_dma.h"
#include "dk1/object_animation_script.h"
#include "dk1/object_frame_dma.h"
#include "dk1/object_frame_layout.h"
#include "dk1/object_screen_oam.h"
#include "dk1/player_coverage.h"
#include "dk1/player_motion.h"
#include "dk1/player_visual_runtime.h"
#include "dk1/player_dispatch.h"
#include "dk1/rom_image.h"
#include "dk1/spc700_bootstrap.h"
#include "dk1/spc700_driver_startup.h"
#include "dk1/spc700_ipl_protocol.h"

int main(int argc, char **argv) {
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom = {0};
    Dk1RomIdentity identity;
    Dk1ApuBootImage apu;
    Dk1Spc700BootstrapCpu spc;
    Dk1Spc700DriverStartup driver;
    Dk1Spc700DriverTrace driver_trace;
    Dk1Spc700IplProtocol ipl;
    Dk1PlayerMotion motion;
    Dk1ObjectAnimationScriptState animation;
    Dk1AnimationScriptResult animation_result;
    Dk1ObjectFrameLayout frame_layout;
    Dk1ObjectFrameOamStyle style = {0x0040u, 0u, 3u, false, false};
    Dk1ObjectScreenTransform transform = {100, 80, 20, 224, 0, 0, 0};
    Dk1ObjectScreenOamStats screen_stats = {0};
    Dk1ObjectFrameDmaStats frame_dma = {0};
    Dk1PlayerVisualStats visual_stats = {0};
    Dk1VramImage visual_base = {{0}}, visual_vram;
    Dk1OamImage visual_oam;
    Dk1NmiVramDmaQueue dma_queue;
    Dk1OamImage oam;
    uint16_t state;
    unsigned invalid = 0u;
    bool spc_ok;
    size_t apu_sources = 0u, apu_bytes = 0u;
    uint64_t catalog_signature, payload_signature, frame_signature = 0u;

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

    if (!dk1_object_frame_layout_read(&rom, animation.frame, &frame_layout) ||
        frame_layout.source_pc != 0xD7620Au || frame_layout.piece_count != 12u) {
        ++invalid;
    } else {
        frame_signature = dk1_object_frame_layout_signature(&frame_layout);
        dk1_oam_init(&oam);
        if (!dk1_object_frame_emit_screen_oam(&frame_layout, transform, style, 0u, 0u,
                                               &oam, &screen_stats) ||
            screen_stats.written != 12u)
            ++invalid;
        dk1_nmi_vram_dma_init(&dma_queue);
        if (!dk1_object_frame_dma_schedule(&frame_layout, style.base_tile_number,
                                           &dma_queue, &frame_dma) ||
            frame_dma.records != 2u || frame_dma.bytes != 576u)
            ++invalid;
        if (!dk1_player_visual_build(&rom, &visual_base, animation.frame, transform,
                                     style, 0u, &visual_oam, &visual_vram, &visual_stats) ||
            visual_stats.pieces != 12u || visual_stats.dma_records != 2u ||
            visual_stats.dma_bytes != 576u)
            ++invalid;
    }

    dk1_spc700_bootstrap_init(&spc, &apu);
    spc_ok = dk1_spc700_bootstrap_send_byte(&spc, 1u, 0x2000u, 0x12u, 256u) &&
             dk1_spc700_bootstrap_send_byte(&spc, 3u, 0x2001u, 0x34u, 256u) &&
             dk1_spc700_bootstrap_launch(&spc, 4u, 0x2000u, 256u);
    if (!spc_ok) ++invalid;

    if (!dk1_spc700_driver_startup_load(&rom, &driver) ||
        !dk1_spc700_driver_trace_entry(&driver, 0x5Au, 64u, &driver_trace) ||
        driver_trace.instructions != 13u ||
        driver_trace.stop != DK1_SPC_TRACE_IPL_BRK_HANDOFF ||
        driver_trace.pc != 0xFFC0u || driver_trace.brk_vector != 0xFFC0u)
        ++invalid;

    memset(&motion, 0, sizeof(motion));
    motion.world_x = 100;
    motion.world_y = 40;
    motion.target_velocity_x = 0x0180;
    dk1_player_motion_approach_horizontal(&motion, 0u);
    dk1_player_motion_apply_gravity(&motion, false, 0);
    dk1_player_motion_integrate(&motion);
    if (motion.world_x != 100 || motion.world_y != 39 ||
        motion.velocity_x != 0x0030 || motion.velocity_y != (int16_t)-0x0070 ||
        motion.subpixel_x != 0x3000u || motion.subpixel_y != 0x9000u)
        ++invalid;

    if (!dk1_spc700_ipl_init(&ipl, driver.ram, sizeof(driver.ram)) ||
        !dk1_spc700_ipl_begin(&ipl, DK1_SPC_IPL_BEGIN, 0x3000u) ||
        !dk1_spc700_ipl_write_byte(&ipl, 0u, 0x11u) ||
        !dk1_spc700_ipl_write_byte(&ipl, 1u, 0x22u) ||
        !dk1_spc700_ipl_write_byte(&ipl, 2u, 0x33u) ||
        !dk1_spc700_ipl_launch(&ipl, 4u, DK1_SPC_DRIVER_ENTRY) ||
        ipl.bytes_written != 3u || ipl.pc != DK1_SPC_DRIVER_ENTRY)
        ++invalid;

    if (!dk1_apu_driver_catalog_validate(&rom, &apu_sources, &apu_bytes)) ++invalid;
    catalog_signature = dk1_apu_driver_catalog_signature(&rom);
    payload_signature = dk1_apu_driver_payload_signature(&rom);
    if (catalog_signature == 0u || payload_signature == 0u) ++invalid;

    printf("player_states=%u planned=%u local=%u invalid=%u "
           "translation=%016llX apu_boot=%016llX animation=%04X "
           "frame_pieces=%u frame_layout=%016llX screen_oam=%u "
           "frame_dma_records=%u frame_dma_bytes=%u visual_pieces=%u visual_dma=%u spc_steps=%llu "
           "driver_code=%016llX driver_data=%016llX driver_ram=%016llX "
           "driver_steps=%llu driver_pc=%04X driver_stop=%u driver_vector=%04X "
           "motion_x=%d motion_y=%d motion_vx=%04X motion_vy=%04X "
           "motion_subx=%04X motion_suby=%04X ipl_bytes=%llu ipl_pc=%04X "
           "apu_sources=%u apu_bytes=%u apu_catalog=%016llX apu_payload=%016llX\n",
           (unsigned)DK1_PLAYER_STATE_COUNT,
           (unsigned)dk1_player_planned_state_count(),
           (unsigned)dk1_player_translated_state_count(),
           invalid,
           (unsigned long long)dk1_player_translation_signature(),
           (unsigned long long)apu.signature,
           animation.frame,
           (unsigned)frame_layout.piece_count,
           (unsigned long long)frame_signature,
           (unsigned)screen_stats.written,
           (unsigned)frame_dma.records,
           (unsigned)frame_dma.bytes,
           (unsigned)visual_stats.pieces,
           (unsigned)visual_stats.dma_bytes,
           (unsigned long long)spc.instructions,
           (unsigned long long)driver.code_signature,
           (unsigned long long)driver.data_signature,
           (unsigned long long)driver.ram_signature,
           (unsigned long long)driver_trace.instructions,
           (unsigned)driver_trace.pc,
           (unsigned)driver_trace.stop,
           (unsigned)driver_trace.brk_vector,
           motion.world_x,
           motion.world_y,
           (unsigned)(uint16_t)motion.velocity_x,
           (unsigned)(uint16_t)motion.velocity_y,
           (unsigned)motion.subpixel_x,
           (unsigned)motion.subpixel_y,
           (unsigned long long)ipl.bytes_written,
           (unsigned)ipl.pc,
           (unsigned)apu_sources,
           (unsigned)apu_bytes,
           (unsigned long long)catalog_signature,
           (unsigned long long)payload_signature);
    dk1_rom_free(&owned);
    return invalid != 0u ? 1 : 0;
}
