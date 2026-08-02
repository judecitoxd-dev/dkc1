#include <assert.h>
#include <stdlib.h>
#include "dk1/level_software_frontend.h"
#include "dk1/preview_asset_warmup.h"

int main(void) {
    const char *path = getenv("DK1_TEST_ROM");
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom = {0};
    Dk1SceneMemory *scene;
    Dk1SoftwareFrontend frontend;
    Dk1LevelSoftwareFrontendStats source_stats;
    Dk1PreviewAssetWarmupStats warmup;
    Dk1Rgba8 *pixels;
    Dk1RgbaSurface surface;

    if (path == NULL)
        path = "/mnt/data/Donkey Kong Country (USA) (Rev 2).sfc";
    assert(dk1_rom_load_file(path, &owned, &rom));
    scene = (Dk1SceneMemory *)malloc(sizeof(*scene));
    pixels = (Dk1Rgba8 *)calloc(384u * 224u, sizeof(*pixels));
    assert(scene != NULL && pixels != NULL);

    assert(dk1_scene_memory_load(&rom, 0x0016u, false, false, scene));
    assert(scene->assets.textures_loaded);
    assert(scene->assets.palettes_loaded);
    assert(scene->assets.package_count != 0u);
    assert(scene->assets.dma_bytes != 0u);

    assert(dk1_level_software_frontend_init(
        &rom, scene, 0x0016u, 384u, 224u,
        &frontend, &source_stats));
    surface = (Dk1RgbaSurface){pixels, 384u, 224u, 384u};
    assert(dk1_preview_asset_warmup(
        &rom, scene, &frontend, surface, &warmup));

    assert(warmup.scene_textures_ready);
    assert(warmup.scene_palettes_ready);
    assert(warmup.dynamic_bg1_ready);
    assert(warmup.player_visual_ready);
    assert(warmup.scene_packages == scene->assets.package_count);
    assert(warmup.scene_dma_bytes == scene->assets.dma_bytes);
    assert(warmup.player_dma_records != 0u);
    assert(warmup.player_dma_bytes != 0u);
    assert(frontend.player_visual_ready);

    dk1_software_frontend_dispose(&frontend);
    free(pixels);
    free(scene);
    dk1_rom_free(&owned);
    return 0;
}
