#include <assert.h>
#include <stdlib.h>
#include "dk1/rom_image.h"
#include "dk1/scene_memory.h"
#include "dk1/scene_signature.h"

int main(void) {
    const char *path = getenv("DK1_TEST_ROM");
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom = {0};
    Dk1SceneMemory *first;
    Dk1SceneMemory *second;

    if (path == NULL)
        path = "/mnt/data/Donkey Kong Country (USA) (Rev 2).sfc";
    assert(dk1_rom_load_file(path, &owned, &rom));

    first = (Dk1SceneMemory *)malloc(sizeof(*first));
    second = (Dk1SceneMemory *)malloc(sizeof(*second));
    assert(first != NULL && second != NULL);
    assert(dk1_scene_memory_load(&rom, 0x0016u, false, false, first));
    assert(dk1_scene_memory_load(&rom, 0x0016u, false, false, second));

    assert(first->assets.textures_loaded);
    assert(first->assets.palettes_loaded);
    assert(first->assets.package_count == first->packages.count);
    assert(first->assets.package_count > 0u);
    assert(first->assets.package_records > 0u);
    assert(first->assets.package_records ==
           first->assets.direct_records + first->assets.compressed_records);
    assert(first->assets.dma_bytes > 0u);
    assert(first->assets.nonzero_vram_bytes > 0u);
    assert(first->assets.palette_uploads ==
           first->recipe.palette_upload_count);
    assert(first->assets.palette_uploads > 0u);
    assert(first->assets.palette_colors > 0u);
    assert(first->assets.nonzero_palette_colors > 0u);
    assert(first->assets.vram_signature != 0u);
    assert(first->assets.palette_signature != 0u);

    assert(first->assets.vram_signature == second->assets.vram_signature);
    assert(first->assets.palette_signature == second->assets.palette_signature);
    assert(first->assets.package_records == second->assets.package_records);
    assert(first->assets.dma_bytes == second->assets.dma_bytes);
    assert(dk1_scene_memory_signature(first) ==
           dk1_scene_memory_signature(second));

    free(second);
    free(first);
    dk1_rom_free(&owned);
    return 0;
}
