#include <assert.h>
#include "dk1/nmi_dispatch.h"

int main(void) {
    Dk1NmiDispatchPlan plan = dk1_nmi_dispatch_plan(0xA98E);
    assert(plan.callback_bank == 0x80);
    assert(plan.acknowledge_nmi);
    assert(plan.force_blank_on_entry);
    assert(plan.kind == DK1_NMI_CALLBACK_RESTORE_DISPLAY_AND_RETURN);
    assert(plan.restore_display_before_rti);
    assert(plan.returns_with_rti);

    plan = dk1_nmi_dispatch_plan(0xA998);
    assert(plan.kind == DK1_NMI_CALLBACK_RETURN_ONLY);
    assert(!plan.restore_display_before_rti);
    assert(plan.returns_with_rti);

    plan = dk1_nmi_dispatch_plan(0x80A9);
    assert(plan.kind == DK1_NMI_CALLBACK_MAIN_FRAME);
    assert(plan.resets_stack_for_frame);
    assert(!plan.returns_with_rti);

    plan = dk1_nmi_dispatch_plan(0x1234);
    assert(plan.kind == DK1_NMI_CALLBACK_UNKNOWN);
    return 0;
}
