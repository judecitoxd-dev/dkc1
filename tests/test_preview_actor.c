#include <assert.h>
#include "dk1/preview_actor.h"

int main(void) {
    uint16_t map[16] = {0};
    uint16_t halves[4] = {0};
    uint16_t flags[64] = {0};
    Dk1TerrainLayout terrain = {map, 1u, halves, 2u, flags};
    Dk1PreviewActor actor = {8, 0, 0, 0, 4, 16, false};
    Dk1PreviewActorConfig config = {1, 3, 1, 2, -8};
    Dk1InputState input = {0};
    unsigned frame = 0u;

    map[15] = 1u;
    halves[2] = 1u;
    halves[3] = 1u;
    for (frame = 0u; frame < 20u && !actor.grounded; ++frame) {
        dk1_input_update_auto(&input, 0u, 0u);
        dk1_input_select_active(&input, 0u, 0u, 0u);
        dk1_preview_actor_step(&actor, config, &input, &terrain);
    }
    assert(actor.grounded);
    assert(actor.y == 15);

    dk1_input_update_auto(&input, DK1_BUTTON_RIGHT | DK1_BUTTON_B, 0u);
    dk1_input_select_active(&input, 0u, 0u, 0u);
    dk1_preview_actor_step(&actor, config, &input, &terrain);
    assert(actor.x > 8);
    assert(actor.velocity_y < 0);
    assert(!actor.grounded);
    return 0;
}
