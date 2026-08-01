#include <assert.h>
#include "dk1/preview_flow.h"

int main(void) {
    Dk1PreviewFlow flow;
    unsigned i;

    dk1_preview_flow_init(&flow, 0x0016u);
    assert(flow.state == DK1_PREVIEW_FLOW_INTRO);
    assert(flow.selected_level == 0x0016u);
    assert(!flow.level_entered);

    for (i = 0u; i < 120u; ++i)
        dk1_preview_flow_step(&flow, 0u, 0u, false);
    assert(flow.state == DK1_PREVIEW_FLOW_INTRO);
    assert(flow.intro_page == 1u);

    dk1_preview_flow_step(&flow, 0u, DK1_HOST_BUTTON_START, false);
    assert(flow.state == DK1_PREVIEW_FLOW_TITLE);
    assert(flow.state_changed);

    dk1_preview_flow_step(&flow, 0u, DK1_HOST_BUTTON_START, false);
    assert(flow.state == DK1_PREVIEW_FLOW_MENU);
    assert(dk1_preview_flow_state_name(flow.state)[0] == 'm');

    dk1_preview_flow_step(&flow, 0u, DK1_HOST_BUTTON_A, false);
    assert(flow.state == DK1_PREVIEW_FLOW_MAP);

    dk1_preview_flow_step(&flow, 0u, DK1_HOST_BUTTON_B, false);
    assert(flow.state == DK1_PREVIEW_FLOW_LEVEL);
    assert(flow.level_entered);
    assert(!flow.level_completed);

    dk1_preview_flow_step(&flow, 0u, 0u, false);
    assert(flow.state == DK1_PREVIEW_FLOW_LEVEL);
    dk1_preview_flow_step(&flow, 0u, 0u, true);
    assert(flow.state == DK1_PREVIEW_FLOW_COMPLETE);
    assert(flow.level_completed);

    dk1_preview_flow_step(&flow, 0u, DK1_HOST_BUTTON_A, false);
    assert(flow.state == DK1_PREVIEW_FLOW_MAP);
    assert(flow.transitions == 6u);
    return 0;
}
