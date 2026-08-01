#ifndef DK1_PREVIEW_FLOW_H
#define DK1_PREVIEW_FLOW_H

#include <stdbool.h>
#include <stdint.h>
#include "dk1/host_input.h"

typedef enum Dk1PreviewFlowState {
    DK1_PREVIEW_FLOW_INTRO = 0,
    DK1_PREVIEW_FLOW_TITLE,
    DK1_PREVIEW_FLOW_MAP,
    DK1_PREVIEW_FLOW_LEVEL,
    DK1_PREVIEW_FLOW_COMPLETE
} Dk1PreviewFlowState;

typedef struct Dk1PreviewFlow {
    Dk1PreviewFlowState state;
    Dk1PreviewFlowState previous_state;
    uint32_t state_frames;
    uint32_t total_frames;
    uint32_t transitions;
    uint16_t selected_level;
    uint8_t intro_page;
    bool state_changed;
    bool level_entered;
    bool level_completed;
} Dk1PreviewFlow;

void dk1_preview_flow_init(Dk1PreviewFlow *flow, uint16_t first_level);
void dk1_preview_flow_step(Dk1PreviewFlow *flow,
                           uint16_t held,
                           uint16_t pressed,
                           bool level_route_completed);
const char *dk1_preview_flow_state_name(Dk1PreviewFlowState state);

#endif
