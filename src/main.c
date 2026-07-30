#include <stdio.h>
#include "dk1/boot_control.h"
#include "dk1/boot_memory.h"
#include "dk1/boot_video.h"
#include "dk1/level_dispatch.h"
#include "dk1/nmi_dispatch.h"
#include "dk1/reset_state.h"
#include "dk1/state_sequence.h"

int main(void) {
    const Dk1BootState state = dk1_boot_state_after_entry();
    const Dk1BootMemoryPlan memory = dk1_boot_memory_plan();
    const Dk1BootControlState control = dk1_boot_control_state(0);
    const Dk1BootVideoConfig video = dk1_boot_video_config();
    const Dk1NmiDispatchPlan nmi = dk1_nmi_dispatch_plan(0x80A9);
    Dk1LevelDispatchEntry level_dispatch = {0};
    size_t upload_count = 0;
    size_t state_count = 0;
    (void)dk1_boot_vram_uploads(&upload_count);
    (void)dk1_initial_boot_sequence(&state_count);
    (void)dk1_level_dispatch_get(0, &level_dispatch);

    puts("dk1 clean-room PC workspace");
    printf("boot model: native=%s irq_disabled=%s fastrom=%s sp=$%04X\n",
           state.native_mode ? "yes" : "no",
           state.interrupts_disabled ? "yes" : "no",
           state.fast_rom_enabled ? "yes" : "no",
           (unsigned)state.stack_pointer);
    printf("early clear plan: WRAM=%u KiB VRAM=%u KiB next=$%06X\n",
           (unsigned)(memory.wram_bytes_cleared / 1024u),
           (unsigned)(memory.vram_bytes_cleared / 1024u),
           (unsigned)memory.continuation_pc);
    printf("boot video: mode=%u forced_blank=$%02X uploads=%u\n",
           (unsigned)video.bgmode,
           (unsigned)video.inidisp,
           (unsigned)upload_count);
    printf("boot control: counter=$%04X init_request=$%04X\n",
           (unsigned)control.boot_counter_003c,
           (unsigned)control.init_request_id);
    printf("state sequencer: initial_callbacks=%u dispatch_slot=$0508\n",
           (unsigned)state_count);
    printf("level dispatch: entries=%u level0=$80:%04X -> $80:%04X\n",
           (unsigned)dk1_level_dispatch_count(),
           (unsigned)level_dispatch.pre_frame_callback_80,
           (unsigned)level_dispatch.frame_callback_80);
    printf("NMI scheduler: bank=$%02X callback=$%04X main_frame=%s\n",
           (unsigned)nmi.callback_bank,
           (unsigned)nmi.callback_address,
           nmi.kind == DK1_NMI_CALLBACK_MAIN_FRAME ? "yes" : "no");
    puts("gameplay status: not implemented");
    return 0;
}
