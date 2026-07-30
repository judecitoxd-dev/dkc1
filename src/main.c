#include <stdio.h>
#include "dk1/boot_control.h"
#include "dk1/boot_memory.h"
#include "dk1/boot_video.h"
#include "dk1/camera.h"
#include "dk1/level_dispatch.h"
#include "dk1/nmi_dispatch.h"
#include "dk1/object_runtime.h"
#include "dk1/reset_state.h"
#include "dk1/state_sequence.h"

int main(void) {
    const Dk1BootState state = dk1_boot_state_after_entry();
    const Dk1BootMemoryPlan memory = dk1_boot_memory_plan();
    const Dk1BootControlState control = dk1_boot_control_state(0);
    const Dk1BootVideoConfig video = dk1_boot_video_config();
    const Dk1NmiDispatchPlan nmi = dk1_nmi_dispatch_plan(0x80A9);
    const Dk1CameraBounds camera_bounds = {0, 0x1000, 0, 0x0200};
    Dk1CameraState camera = {-1, -1, 1u, 1u};
    Dk1LevelDispatchEntry level_dispatch = {0};
    Dk1ObjectTypeDispatch object_type = {0};
    size_t upload_count = 0;
    size_t state_count = 0;

    (void)dk1_boot_vram_uploads(&upload_count);
    (void)dk1_initial_boot_sequence(&state_count);
    (void)dk1_level_dispatch_get(0u, &level_dispatch);
    (void)dk1_object_type_dispatch_get(0u, &object_type);
    dk1_camera_clamp_original(&camera, camera_bounds, 0u);

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
    printf("object runtime: slots=%u types=%u first_callback=$%02X:%04X\n",
           (unsigned)DK1_OBJECT_SLOT_COUNT,
           (unsigned)dk1_object_type_count(),
           (unsigned)DK1_OBJECT_CALLBACK_BANK,
           (unsigned)object_type.callback_address_bf);
    printf("camera model: origin=(%d,%d) SNES viewport=%ux%u widescreen-ready=yes\n",
           (int)camera.x,
           (int)camera.y,
           (unsigned)DK1_SNES_VIEWPORT_WIDTH,
           (unsigned)DK1_SNES_VIEWPORT_HEIGHT);
    puts("gameplay status: runtime foundations implemented; playable scene not yet implemented");
    return 0;
}
