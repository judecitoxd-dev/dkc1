#include "dk1/preview_flow.h"
#include <string.h>

#define DK1_PREVIEW_INTRO_PAGE_FRAMES 120u
#define DK1_PREVIEW_INTRO_PAGE_COUNT 3u

static bool confirm_pressed(uint16_t pressed) {
    const uint16_t mask = DK1_HOST_BUTTON_START |
                          DK1_HOST_BUTTON_A |
                          DK1_HOST_BUTTON_B;
    return (pressed & mask) != 0u;
}

static void enter_state(Dk1PreviewFlow *flow, Dk1PreviewFlowState state) {
    flow->previous_state = flow->state;
    flow->state = state;
    flow->state_frames = 0u;
    flow->state_changed = true;
    ++flow->transitions;
    if (state == DK1_PREVIEW_FLOW_LEVEL)
        flow->level_entered = true;
}

void dk1_preview_flow_init(Dk1PreviewFlow *flow, uint16_t first_level) {
    if (flow == NULL)
        return;
    memset(flow, 0, sizeof(*flow));
    flow->state = DK1_PREVIEW_FLOW_INTRO;
    flow->previous_state = DK1_PREVIEW_FLOW_INTRO;
    flow->selected_level = first_level;
}

void dk1_preview_flow_step(Dk1PreviewFlow *flow,
                           uint16_t held,
                           uint16_t pressed,
                           bool level_route_completed) {
    const uint32_t intro_frames =
        DK1_PREVIEW_INTRO_PAGE_FRAMES * DK1_PREVIEW_INTRO_PAGE_COUNT;
    (void)held;
    if (flow == NULL)
        return;

    flow->state_changed = false;
    ++flow->state_frames;
    ++flow->total_frames;

    if (flow->state == DK1_PREVIEW_FLOW_INTRO) {
        uint32_t page = flow->state_frames / DK1_PREVIEW_INTRO_PAGE_FRAMES;
        if (page >= DK1_PREVIEW_INTRO_PAGE_COUNT)
            page = DK1_PREVIEW_INTRO_PAGE_COUNT - 1u;
        flow->intro_page = (uint8_t)page;
        if (confirm_pressed(pressed) || flow->state_frames >= intro_frames)
            enter_state(flow, DK1_PREVIEW_FLOW_TITLE);
        return;
    }

    if (flow->state == DK1_PREVIEW_FLOW_TITLE) {
        if (confirm_pressed(pressed))
            enter_state(flow, DK1_PREVIEW_FLOW_MAP);
        return;
    }

    if (flow->state == DK1_PREVIEW_FLOW_MAP) {
        if (confirm_pressed(pressed)) {
            flow->level_completed = false;
            enter_state(flow, DK1_PREVIEW_FLOW_LEVEL);
        }
        return;
    }

    if (flow->state == DK1_PREVIEW_FLOW_LEVEL) {
        if (level_route_completed) {
            flow->level_completed = true;
            enter_state(flow, DK1_PREVIEW_FLOW_COMPLETE);
        }
        return;
    }

    if (flow->state == DK1_PREVIEW_FLOW_COMPLETE && confirm_pressed(pressed))
        enter_state(flow, DK1_PREVIEW_FLOW_MAP);
}

const char *dk1_preview_flow_state_name(Dk1PreviewFlowState state) {
    switch (state) {
    case DK1_PREVIEW_FLOW_INTRO: return "intro";
    case DK1_PREVIEW_FLOW_TITLE: return "title";
    case DK1_PREVIEW_FLOW_MAP: return "map";
    case DK1_PREVIEW_FLOW_LEVEL: return "level";
    case DK1_PREVIEW_FLOW_COMPLETE: return "complete";
    default: return "unknown";
    }
}
