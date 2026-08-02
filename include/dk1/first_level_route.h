#ifndef DK1_FIRST_LEVEL_ROUTE_H
#define DK1_FIRST_LEVEL_ROUTE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/scene_memory.h"

struct Dk1SoftwareFrontend;
struct Dk1PreviewAssetWarmupStats;

typedef struct Dk1FirstLevelRoute {
    uint32_t start_x;
    uint32_t exit_x;
    uint32_t current_x;
    uint32_t furthest_x;
    uint32_t frames;
    uint8_t progress_percent;
    uint8_t checkpoints_reached;
    bool completed;
} Dk1FirstLevelRoute;

#define DK1_FIRST_LEVEL_PREFLIGHT_SCENE_TEXTURES (1u << 0)
#define DK1_FIRST_LEVEL_PREFLIGHT_SCENE_PALETTES (1u << 1)
#define DK1_FIRST_LEVEL_PREFLIGHT_SCENE_PACKAGES (1u << 2)
#define DK1_FIRST_LEVEL_PREFLIGHT_SOURCE_ROM (1u << 3)
#define DK1_FIRST_LEVEL_PREFLIGHT_PLAYER_TERRAIN (1u << 4)
#define DK1_FIRST_LEVEL_PREFLIGHT_OBJECT_STREAM (1u << 5)
#define DK1_FIRST_LEVEL_PREFLIGHT_DYNAMIC_BG1 (1u << 6)
#define DK1_FIRST_LEVEL_PREFLIGHT_PLAYER_VISUAL (1u << 7)
#define DK1_FIRST_LEVEL_PREFLIGHT_VIEW (1u << 8)
#define DK1_FIRST_LEVEL_PREFLIGHT_ROUTE (1u << 9)

typedef struct Dk1FirstLevelPreflight {
    uint32_t failure_mask;
    uint32_t route_distance;
    size_t scene_packages;
    size_t scene_dma_bytes;
    size_t initial_map_columns;
    size_t initial_tile_words;
    size_t player_dma_records;
    size_t player_dma_bytes;
    bool ready;
} Dk1FirstLevelPreflight;

/* Portable completion gate for the first public test. The original level-exit
 * callback is still pending; this tracker derives a stable end point from the
 * translated camera bounds so the whole current route can be tested now. */
bool dk1_first_level_route_init(const Dk1SceneMemory *scene,
                                uint16_t viewport_width,
                                uint32_t player_start_x,
                                Dk1FirstLevelRoute *route);

/* Progress uses a high-water mark, so walking backwards never lowers the
 * displayed route percentage. checkpoints_reached reports the 25/50/75/100
 * percent milestones crossed during the current run. */
void dk1_first_level_route_step(Dk1FirstLevelRoute *route,
                                uint32_t player_x);

/* Verifies that all systems needed by the narrow Windows tester are prepared
 * before the window opens: ROM-derived scene graphics and palettes, terrain,
 * object streaming, dynamic BG1, player frame DMA, view and route bounds. */
bool dk1_first_level_preflight(
    const Dk1SceneMemory *scene,
    const struct Dk1SoftwareFrontend *frontend,
    const struct Dk1PreviewAssetWarmupStats *warmup,
    const Dk1FirstLevelRoute *route,
    Dk1FirstLevelPreflight *result);

#endif
