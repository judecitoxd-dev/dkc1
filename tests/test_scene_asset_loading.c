#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "dk1/rom_image.h"
#include "dk1/scene_asset_cache.h"
#include "dk1/scene_memory.h"
#include "dk1/scene_signature.h"

int main(void) {
    static const char cache_path[] = "test_jungle_scene_assets.bin";
    const char *path = getenv("DK1_TEST_ROM");
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom = {0};
    Dk1SceneMemory *first;
    Dk1SceneMemory *second;

    if (path == NULL)
        path = "/mnt/data/Donkey Kong Country (USA) (Rev 2).sfc";
    assert(dk1_rom_load_file(path, &owned, &rom));
    (void)remove(cache_path);

    first = (Dk1SceneMemory *)malloc(sizeof(*first));
    second = (Dk1SceneMemory *)malloc(sizeof(*second));
    assert(first != NULL && second != NULL);
    assert(dk1_scene_memory_load_cached(&rom, 0x0016u,
                                        false, false, cache_path, first));
    assert(first->cache.attempted);
    assert(!first->cache.hit);
    assert(first->cache.written);
    assert(first->cache.bytes_written > DK1_VRAM_BYTE_COUNT);

    assert(dk1_scene_memory_load_cached(&rom, 0x0016u,
                                        false, false, cache_path, second));
    assert(second->cache.attempted);
    assert(second->cache.file_found);
    assert(second->cache.hit);
    assert(!second->cache.written);
    assert(second->cache.bytes_read > DK1_VRAM_BYTE_COUNT);
    assert(first->cache.rom_signature == second->cache.rom_signature);

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

    assert(remove(cache_path) == 0);
    free(second);
    free(first);
    dk1_rom_free(&owned);
    return 0;
}
