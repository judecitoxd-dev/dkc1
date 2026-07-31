#include <assert.h>
#include "dk1/player_dispatch.h"
int main(void) {
    Dk1PlayerStateDispatch e;
    assert(dk1_player_state_count() == 87u);
    assert(dk1_player_state_dispatch_get(0u, &e) && e.handler_bf == 0x8756u);
    assert(dk1_player_state_dispatch_get(42u, &e) && e.handler_bf == 0x9758u);
    assert(dk1_player_state_dispatch_get(86u, &e) && e.handler_bf == 0xA023u);
    assert(!dk1_player_state_dispatch_get(87u, &e));
    assert(dk1_player_state_handler_pc(0u) == 0xBF8756u);
    assert(dk1_player_object_callback_pc(0u) == 0xBF84ABu);
    assert(dk1_player_object_callback_pc(1u) == 0xBF84A4u);
    assert(dk1_player_object_callback_pc(2u) == 0u);
    return 0;
}
