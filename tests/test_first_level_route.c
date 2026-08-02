#include <assert.h>
#include <string.h>
#include "dk1/first_level_route.h"
#include "dk1/preview_asset_warmup.h"
#include "dk1/software_frontend.h"

static void test_route_progress(void) {
    Dk1SceneMemory scene;
    Dk1FirstLevelRoute route;
    uint32_t midpoint;

    memset(&scene, 0, sizeof(scene));
    scene.camera.maximum_x = 1000u;
    assert(dk1_first_level_route_init(&scene, 384u, 128u, &route));
    assert(route.start_x == 128u);
    assert(route.furthest_x == 128u);
    assert(route.exit_x == 1320u);
    assert(route.progress_percent == 0u);
    assert(route.checkpoints_reached == 0u);
    assert(!route.completed);

    dk1_first_level_route_step(&route, 64u);
    assert(route.progress_percent == 0u);
    assert(route.furthest_x == 128u);
    assert(!route.completed);

    midpoint = route.start_x + (route.exit_x - route.start_x) / 2u;
    dk1_first_level_route_step(&route, midpoint);
    assert(route.progress_percent >= 49u);
    assert(route.progress_percent <= 50u);
    assert(route.checkpoints_reached == 2u);
    assert(!route.completed);

    /* Backtracking must not lower the tester's route progress. */
    dk1_first_level_route_step(&route, route.start_x + 8u);
    assert(route.progress_percent >= 49u);
    assert(route.progress_percent <= 50u);
    assert(route.furthest_x == midpoint);
    assert(route.checkpoints_reached == 2u);

    dk1_first_level_route_step(&route, route.exit_x - 1u);
    assert(route.progress_percent == 99u);
    assert(route.checkpoints_reached == 3u);
    assert(!route.completed);

    dk1_first_level_route_step(&route, route.exit_x);
    assert(route.progress_percent == 100u);
    assert(route.checkpoints_reached == 4u);
    assert(route.completed);
    assert(route.frames == 5u);
}

static void test_preview_unblank(void) {
    Dk1SceneMemory scene;
    const size_t display_index =
        (size_t)(0x2100u - DK1_PPU_REGISTER_FIRST);

    memset(&scene, 0, sizeof(scene));
    assert(dk1_preview_asset_unblank_scene(&scene));
    assert(scene.ppu.written[display_index]);
    assert(scene.ppu.values[display_index] == 0x0Fu);
    assert(scene.ppu.write_count == 1u);
    assert(!dk1_preview_asset_unblank_scene(&scene));
    assert(scene.ppu.write_count == 1u);

    scene.ppu.values[display_index] = 0x8Fu;
    assert(dk1_preview_asset_unblank_scene(&scene));
    assert(scene.ppu.values[display_index] == 0x0Fu);
    assert(scene.ppu.write_count == 1u);

    scene.ppu.values[display_index] = 0x07u;
    assert(!dk1_preview_asset_unblank_scene(&scene));
    assert(scene.ppu.values[display_index] == 0x07u);
}

static void test_startup_preflight(void) {
    Dk1SceneMemory scene;
    Dk1RomImage source_rom;
    Dk1FirstLevelRoute route;
    Dk1SoftwareFrontend frontend;
    Dk1PreviewAssetWarmupStats warmup;
    Dk1DynamicBg1Runtime bg1;
    Dk1FirstLevelPreflight preflight;

    memset(&scene, 0, sizeof(scene));
    memset(&source_rom, 0, sizeof(source_rom));
    memset(&frontend, 0, sizeof(frontend));
    memset(&warmup, 0, sizeof(warmup));
    memset(&bg1, 0, sizeof(bg1));

    scene.camera.maximum_x = 1000u;
    scene.assets.package_count = 3u;
    scene.assets.dma_bytes = 4096u;
    scene.assets.textures_loaded = true;
    scene.assets.palettes_loaded = true;
    assert(dk1_first_level_route_init(&scene, 384u, 128u, &route));

    frontend.runtime.view.width = 384u;
    frontend.runtime.view.height = 224u;
    frontend.dynamic_bg1 = &bg1;
    frontend.source_rom = &source_rom;
    frontend.player_terrain_ready = true;
    frontend.level_objects_ready = true;
    bg1.ready = true;
    bg1.map_columns_uploaded = 12u;
    bg1.tile_words_uploaded = 3072u;

    warmup.scene_packages = 3u;
    warmup.scene_dma_bytes = 4096u;
    warmup.player_dma_records = 2u;
    warmup.player_dma_bytes = 512u;
    warmup.scene_textures_ready = true;
    warmup.scene_palettes_ready = true;
    warmup.dynamic_bg1_ready = true;
    warmup.player_visual_ready = true;
    warmup.visible_pixels = 100u;
    warmup.frame_signature = 1u;
    warmup.visible_frame_ready = true;

    assert(dk1_first_level_preflight(
        &scene, &frontend, &warmup, &route, &preflight));
    assert(preflight.ready);
    assert(preflight.failure_mask == 0u);
    assert(preflight.route_distance == 1192u);
    assert(preflight.initial_map_columns == 12u);
    assert(preflight.initial_tile_words == 3072u);

    warmup.player_visual_ready = false;
    frontend.level_objects_ready = false;
    assert(!dk1_first_level_preflight(
        &scene, &frontend, &warmup, &route, &preflight));
    assert((preflight.failure_mask &
            DK1_FIRST_LEVEL_PREFLIGHT_PLAYER_VISUAL) != 0u);
    assert((preflight.failure_mask &
            DK1_FIRST_LEVEL_PREFLIGHT_OBJECT_STREAM) != 0u);
}

int main(void) {
    test_route_progress();
    test_preview_unblank();
    test_startup_preflight();
    return 0;
}
