#ifndef DK1_LEVEL_SOFTWARE_FRONTEND_H
#define DK1_LEVEL_SOFTWARE_FRONTEND_H

#include <stdbool.h>
#include <stdint.h>
#include "dk1/level_object_spawn.h"
#include "dk1/software_frontend.h"

typedef struct Dk1LevelSoftwareFrontendStats {
    Dk1LevelObjectSpawn barrel_spawn;
    uint16_t entrance_id;
    bool barrel_found;
    bool barrel_spawned;
} Dk1LevelSoftwareFrontendStats;

/* Initializes the regular software frontend and then imports the first normal
 * Barrel object directly from the entrance's original eight-byte sprite list.
 * Entrances without a normal Barrel still initialize successfully. */
bool dk1_level_software_frontend_init(const Dk1RomImage *rom,
                                      const Dk1SceneMemory *scene,
                                      uint16_t entrance_id,
                                      uint16_t width,
                                      uint16_t height,
                                      Dk1SoftwareFrontend *frontend,
                                      Dk1LevelSoftwareFrontendStats *stats);

#endif
