#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "dk1/scene_asset_cache.h"

int main(void) {
    static const char cache_path[] = "test_scene_asset_cache.bin";
    uint8_t rom_bytes[64];
    Dk1RomImage rom;
    Dk1SceneAssetCacheKey key;
    Dk1SceneAssetCacheKey wrong_key;
    Dk1SceneAssetCacheIoStats io;
    Dk1SceneMemory source;
    Dk1SceneMemory restored;
    char generated_path[256];
    FILE *file;
    long final_offset;
    int final_byte;
    size_t i;

    (void)remove(cache_path);
    for (i = 0u; i < sizeof(rom_bytes); ++i)
        rom_bytes[i] = (uint8_t)(i * 7u + 3u);
    assert(dk1_rom_image_init(&rom, rom_bytes, sizeof(rom_bytes)));
    assert(dk1_scene_asset_cache_key(&rom, 0x0016u, false, false, &key));
    assert(key.rom_size == sizeof(rom_bytes));
    assert(key.level_id == 0x0016u);
    assert(key.options == 0u);
    assert(dk1_scene_asset_cache_default_path(
        "legal.sfc", 0x0016u, false, true,
        generated_path, sizeof(generated_path)));
    assert(strcmp(generated_path,
                  "legal.sfc.dk1-assets-0016-01.bin") == 0);

    memset(&source, 0, sizeof(source));
    source.packages.count = 2u;
    source.recipe.palette_upload_count = 3u;
    source.assets.package_count = 2u;
    source.assets.package_records = 5u;
    source.assets.direct_records = 2u;
    source.assets.compressed_records = 3u;
    source.assets.dma_bytes = 4096u;
    source.assets.decompressed_bytes = 6144u;
    source.assets.palette_uploads = 3u;
    source.assets.palette_colors = 48u;
    for (i = 0u; i < sizeof(source.vram.bytes); ++i)
        source.vram.bytes[i] = (uint8_t)((i * 13u) ^ (i >> 4u));
    for (i = 0u; i < DK1_CGRAM_COLOR_COUNT; ++i)
        source.cgram.colors[i] = (uint16_t)((i * 97u) & 0x7FFFu);

    assert(dk1_scene_asset_cache_write(cache_path, &key, &source, &io));
    assert(io.bytes_written > DK1_VRAM_BYTE_COUNT);

    memset(&restored, 0, sizeof(restored));
    restored.packages.count = source.packages.count;
    restored.recipe.palette_upload_count =
        source.recipe.palette_upload_count;
    assert(dk1_scene_asset_cache_read(cache_path, &key, &restored, &io));
    assert(io.file_found);
    assert(!io.rejected);
    assert(io.bytes_read == io.bytes_written || io.bytes_read > 0u);
    assert(memcmp(restored.vram.bytes, source.vram.bytes,
                  sizeof(source.vram.bytes)) == 0);
    assert(memcmp(restored.cgram.colors, source.cgram.colors,
                  sizeof(source.cgram.colors)) == 0);
    assert(restored.assets.package_records == 5u);
    assert(restored.assets.direct_records == 2u);
    assert(restored.assets.compressed_records == 3u);
    assert(restored.assets.textures_loaded);
    assert(restored.assets.palettes_loaded);
    assert(restored.assets.vram_signature != 0u);
    assert(restored.assets.palette_signature != 0u);

    wrong_key = key;
    ++wrong_key.level_id;
    memset(&restored, 0, sizeof(restored));
    restored.packages.count = source.packages.count;
    restored.recipe.palette_upload_count =
        source.recipe.palette_upload_count;
    assert(!dk1_scene_asset_cache_read(cache_path, &wrong_key,
                                       &restored, &io));
    assert(io.file_found);
    assert(io.rejected);

    file = fopen(cache_path, "r+b");
    assert(file != NULL);
    assert(fseek(file, -1L, SEEK_END) == 0);
    final_offset = ftell(file);
    assert(final_offset >= 0L);
    final_byte = fgetc(file);
    assert(final_byte != EOF);
    assert(fseek(file, final_offset, SEEK_SET) == 0);
    assert(fputc(final_byte ^ 0x5Au, file) != EOF);
    assert(fclose(file) == 0);

    memset(&restored, 0, sizeof(restored));
    restored.packages.count = source.packages.count;
    restored.recipe.palette_upload_count =
        source.recipe.palette_upload_count;
    assert(!dk1_scene_asset_cache_read(cache_path, &key, &restored, &io));
    assert(io.file_found);
    assert(io.rejected);

    assert(remove(cache_path) == 0);
    return 0;
}
