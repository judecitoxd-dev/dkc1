#ifndef DK1_SOFTWARE_FRONTEND_H
#define DK1_SOFTWARE_FRONTEND_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/barrel_scene_runtime.h"
#include "dk1/dynamic_stream.h"
#include "dk1/gnawty_runtime.h"
#include "dk1/host_input.h"
#include "dk1/level_object_stream.h"
#include "dk1/oam_image.h"
#include "dk1/player_live_runtime.h"
#include "dk1/player_preview_runtime.h"
#include "dk1/player_terrain_runtime.h"
#include "dk1/player_visual_runtime.h"
#include "dk1/ppu_compositor.h"
#include "dk1/scene_runtime.h"

typedef struct Dk1SoftwareFrontend {
    Dk1SceneRuntime runtime;
    Dk1HostInputState input;
    Dk1OamImage oam;
    Dk1VramImage player_vram;
    Dk1PlayerVisualStats player_visual;
    Dk1PlayerPreviewRuntime player_preview;
    Dk1PlayerLiveRuntime player_live;
    Dk1PlayerTerrainRuntime player_terrain;
    Dk1BarrelSceneRuntime barrel;
    Dk1GnawtyRuntime gnawty;
    Dk1DynamicStreamState stream;
    Dk1LevelObjectStream level_objects;
    const Dk1RomImage *source_rom;
    size_t gnawty_record_index;
    uint8_t obsel;
    int16_t marker_x, marker_y;
    uint16_t marker_tile;
    uint8_t marker_palette;
    uint16_t player_frame, player_tile_base, player_vertical_origin;
    bool player_visual_ready, player_terrain_ready, barrel_ready;
    bool level_objects_ready, gnawty_ready;
    bool defeated_level_records[DK1_LEVEL_OBJECT_STREAM_MAX_ENTRIES];
} Dk1SoftwareFrontend;

bool dk1_software_frontend_init(const Dk1SceneMemory *, uint16_t, uint16_t,
                                Dk1SoftwareFrontend *);
bool dk1_software_frontend_init_with_rom(const Dk1RomImage *,
                                         const Dk1SceneMemory *,
                                         uint16_t, uint16_t,
                                         Dk1SoftwareFrontend *);
bool dk1_software_frontend_spawn_barrel(Dk1SoftwareFrontend *,
                                        uint16_t type_id,
                                        uint16_t world_x,
                                        int16_t world_y);
/* Connects the first currently active source Gnawty to its executable runtime.
 * The camera stream remains authoritative for source record and slot lifetime. */
bool dk1_software_frontend_sync_gnawty(Dk1SoftwareFrontend *);
bool dk1_software_frontend_step(const Dk1SceneMemory *, uint16_t,
                                Dk1SoftwareFrontend *);
bool dk1_software_frontend_render(const Dk1RomImage *, const Dk1SceneMemory *,
                                  Dk1SoftwareFrontend *, Dk1RgbaSurface);
uint64_t dk1_software_frontend_signature(const Dk1SoftwareFrontend *);

#endif
