#ifndef DK1_LEVEL_SOFTWARE_FRONTEND_H
#define DK1_LEVEL_SOFTWARE_FRONTEND_H

#include <stdbool.h>
#include <stdint.h>
#include "dk1/level_object_import.h"
#include "dk1/level_object_spawn.h"
#include "dk1/software_frontend.h"

typedef struct Dk1LevelSoftwareFrontendStats {
    Dk1LevelObjectImport primary_import;
    Dk1LevelObjectSpawn barrel_spawn;
    uint16_t entrance_id;
    bool object_list_imported;
    bool barrel_found;
    bool barrel_spawned;
} Dk1LevelSoftwareFrontendStats;

/* Initializes the regular software frontend, imports the entrance's complete
 * normal-record window into the 25-slot primary scheduler, and then connects
 * the first normal Barrel to the currently executable scene runtime. Entrances
 * without a resolvable object list or normal Barrel still initialize. */
bool dk1_level_software_frontend_init(const Dk1RomImage *rom,
                                      const Dk1SceneMemory *scene,
                                      uint16_t entrance_id,
                                      uint16_t width,
                                      uint16_t height,
                                      Dk1SoftwareFrontend *frontend,
                                      Dk1LevelSoftwareFrontendStats *stats);

#endif
