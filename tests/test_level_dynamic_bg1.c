#include <assert.h>
#include <stdlib.h>
#include "dk1/level_software_frontend.h"
#include "dk1/scene_signature.h"

int main(void) {
    const char *path = getenv("DK1_TEST_ROM");
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom = {0};
    Dk1SceneMemory *scene;
    Dk1SoftwareFrontend frontend;
    Dk1LevelSoftwareFrontendStats stats;
    Dk1Rgba8 *pixels;
    Dk1RgbaSurface surface;
    uint64_t initial_signature;
    size_t initial_columns;
    size_t expected_columns;
    size_t frame;

    if (path == NULL)
        path = "/mnt/data/Donkey Kong Country (USA) (Rev 2).sfc";
    assert(dk1_rom_load_file(path, &owned, &rom));
    scene = (Dk1SceneMemory *)malloc(sizeof(*scene));
    pixels = (Dk1Rgba8 *)calloc(384u * 224u, sizeof(*pixels));
    assert(scene != NULL && pixels != NULL);
    assert(dk1_scene_memory_load(&rom, 0x0016u, false, false, scene));
    assert(dk1_level_software_frontend_init(
        &rom, scene, 0x0016u, 384u, 224u, &frontend, &stats));

    expected_columns = (size_t)frontend.stream.last_column -
                       frontend.stream.first_column + 1u;
    assert(frontend.dynamic_bg1 != NULL);
    assert(frontend.dynamic_bg1->ready);
    assert(frontend.dynamic_bg1->update_count == 1u);
    assert(frontend.dynamic_bg1->map_columns_uploaded == expected_columns);
    assert(frontend.dynamic_bg1->tile_columns_uploaded ==
           expected_columns * 4u);
    assert(frontend.dynamic_bg1->tile_words_uploaded > 0u);
    initial_columns = frontend.dynamic_bg1->map_columns_uploaded;
    initial_signature =
        dk1_dynamic_bg1_runtime_signature(frontend.dynamic_bg1);
    assert(initial_signature != 0u);

    for (frame = 0u; frame < 16u; ++frame)
        assert(dk1_software_frontend_step(
            scene, DK1_RUNTIME_RIGHT, &frontend));
    assert(frontend.stream.generation > 1u);
    assert(frontend.dynamic_bg1->generation > 1u);
    assert(frontend.dynamic_bg1->map_columns_uploaded > initial_columns);
    assert(dk1_dynamic_bg1_runtime_signature(frontend.dynamic_bg1) !=
           initial_signature);

    surface = (Dk1RgbaSurface){pixels, 384u, 224u, 384u};
    assert(dk1_software_frontend_render(
        &rom, scene, &frontend, surface));

    dk1_software_frontend_dispose(&frontend);
    free(pixels);
    free(scene);
    dk1_rom_free(&owned);
    return 0;
}
