#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "dk1/software_frontend.h"

int main(void) {
    Dk1SoftwareFrontend frontend;
    size_t i;

    memset(&frontend, 0, sizeof(frontend));
    frontend.gnawty.active = true;
    frontend.gnawty_ready = true;
    frontend.gnawty_active_count = 3u;
    frontend.gnawty_capacity_overflow_count = 2u;

    for (i = 0u; i < 2u; ++i) {
        Dk1SoftwareGnawtySlot *slot = &frontend.additional_gnawties[i];
        slot->runtime = (Dk1GnawtyRuntime *)calloc(1u,
                                                  sizeof(*slot->runtime));
        assert(slot->runtime != NULL);
        slot->runtime->active = true;
        slot->record_index = 10u + i;
        slot->ready = true;
    }

    dk1_software_frontend_dispose(&frontend);
    assert(!frontend.gnawty.active);
    assert(!frontend.gnawty_ready);
    assert(frontend.gnawty_active_count == 0u);
    for (i = 0u; i < DK1_SOFTWARE_FRONTEND_EXTRA_GNAWTIES; ++i) {
        assert(frontend.additional_gnawties[i].runtime == NULL);
        assert(!frontend.additional_gnawties[i].ready);
        assert(frontend.additional_gnawties[i].record_index == 0u);
    }

    /* Disposal is deliberately safe before initialization and after cleanup. */
    dk1_software_frontend_dispose(&frontend);
    assert(frontend.gnawty_active_count == 0u);
    return 0;
}
