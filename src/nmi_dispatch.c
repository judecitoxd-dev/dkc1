#include "dk1/nmi_dispatch.h"

Dk1NmiDispatchPlan dk1_nmi_dispatch_plan(uint16_t callback_address) {
    Dk1NmiDispatchPlan plan = {
        .callback_address = callback_address,
        .callback_bank = 0x80,
        .kind = DK1_NMI_CALLBACK_UNKNOWN,
        .acknowledge_nmi = true,
        .force_blank_on_entry = true,
        .restore_display_before_rti = false,
        .resets_stack_for_frame = false,
        .returns_with_rti = false,
    };

    switch (callback_address) {
        case 0xA98E:
            plan.kind = DK1_NMI_CALLBACK_RESTORE_DISPLAY_AND_RETURN;
            plan.restore_display_before_rti = true;
            plan.returns_with_rti = true;
            break;
        case 0xA998:
            plan.kind = DK1_NMI_CALLBACK_RETURN_ONLY;
            plan.returns_with_rti = true;
            break;
        case 0x80A9:
            plan.kind = DK1_NMI_CALLBACK_MAIN_FRAME;
            plan.resets_stack_for_frame = true;
            break;
        default:
            break;
    }

    return plan;
}
