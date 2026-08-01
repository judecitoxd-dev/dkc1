#include "dk1/scene_asset_cache.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DK1_SCENE_ASSET_CACHE_MAGIC_SIZE 8u
#define DK1_SCENE_ASSET_CACHE_VRAM_SIZE DK1_VRAM_BYTE_COUNT
#define DK1_SCENE_ASSET_CACHE_PALETTE_BYTES \
    (DK1_CGRAM_COLOR_COUNT * 2u)

static const uint8_t dk1_scene_asset_cache_magic[
    DK1_SCENE_ASSET_CACHE_MAGIC_SIZE] = {
        'D', 'K', '1', 'A', 'S', 'S', 'T', '1'
    };

static uint64_t hash_continue(const void *data, size_t size, uint64_t hash) {
    const uint8_t *bytes = (const uint8_t *)data;
    size_t i;
    if (hash == 0u)
        hash = UINT64_C(1469598103934665603);
    for (i = 0u; i < size; ++i) {
        hash ^= bytes[i];
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

static uint64_t hash_bytes(const void *data, size_t size) {
    return hash_continue(data, size, 0u);
}

static void palette_to_bytes(const Dk1CgramImage *cgram,
                             uint8_t bytes[DK1_SCENE_ASSET_CACHE_PALETTE_BYTES]) {
    size_t i;
    for (i = 0u; i < DK1_CGRAM_COLOR_COUNT; ++i) {
        const uint16_t color = cgram->colors[i];
        bytes[i * 2u] = (uint8_t)color;
        bytes[i * 2u + 1u] = (uint8_t)(color >> 8u);
    }
}

static void palette_from_bytes(
    const uint8_t bytes[DK1_SCENE_ASSET_CACHE_PALETTE_BYTES],
    Dk1CgramImage *cgram) {
    size_t i;
    for (i = 0u; i < DK1_CGRAM_COLOR_COUNT; ++i) {
        cgram->colors[i] = (uint16_t)bytes[i * 2u] |
            ((uint16_t)bytes[i * 2u + 1u] << 8u);
    }
}

static size_t count_nonzero_bytes(const uint8_t *bytes, size_t size) {
    size_t count = 0u;
    size_t i;
    for (i = 0u; i < size; ++i) {
        if (bytes[i] != 0u)
            ++count;
    }
    return count;
}

static size_t count_nonzero_colors(const Dk1CgramImage *cgram) {
    size_t count = 0u;
    size_t i;
    for (i = 0u; i < DK1_CGRAM_COLOR_COUNT; ++i) {
        if ((cgram->colors[i] & 0x7FFFu) != 0u)
            ++count;
    }
    return count;
}

static bool write_bytes(FILE *file, const void *data, size_t size,
                        Dk1SceneAssetCacheIoStats *stats) {
    if (file == NULL || data == NULL || fwrite(data, 1u, size, file) != size)
        return false;
    if (stats != NULL)
        stats->bytes_written += size;
    return true;
}

static bool write_u8(FILE *file, uint8_t value,
                     Dk1SceneAssetCacheIoStats *stats) {
    return write_bytes(file, &value, sizeof(value), stats);
}

static bool write_u16(FILE *file, uint16_t value,
                      Dk1SceneAssetCacheIoStats *stats) {
    const uint8_t bytes[2] = {(uint8_t)value, (uint8_t)(value >> 8u)};
    return write_bytes(file, bytes, sizeof(bytes), stats);
}

static bool write_u32(FILE *file, uint32_t value,
                      Dk1SceneAssetCacheIoStats *stats) {
    const uint8_t bytes[4] = {
        (uint8_t)value,
        (uint8_t)(value >> 8u),
        (uint8_t)(value >> 16u),
        (uint8_t)(value >> 24u)
    };
    return write_bytes(file, bytes, sizeof(bytes), stats);
}

static bool write_u64(FILE *file, uint64_t value,
                      Dk1SceneAssetCacheIoStats *stats) {
    uint8_t bytes[8];
    size_t i;
    for (i = 0u; i < sizeof(bytes); ++i)
        bytes[i] = (uint8_t)(value >> (i * 8u));
    return write_bytes(file, bytes, sizeof(bytes), stats);
}

static bool read_bytes(FILE *file, void *data, size_t size,
                       Dk1SceneAssetCacheIoStats *stats) {
    if (file == NULL || data == NULL || fread(data, 1u, size, file) != size)
        return false;
    if (stats != NULL)
        stats->bytes_read += size;
    return true;
}

static bool read_u8(FILE *file, uint8_t *value,
                    Dk1SceneAssetCacheIoStats *stats) {
    return read_bytes(file, value, sizeof(*value), stats);
}

static bool read_u16(FILE *file, uint16_t *value,
                     Dk1SceneAssetCacheIoStats *stats) {
    uint8_t bytes[2];
    if (!read_bytes(file, bytes, sizeof(bytes), stats))
        return false;
    *value = (uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8u);
    return true;
}

static bool read_u32(FILE *file, uint32_t *value,
                     Dk1SceneAssetCacheIoStats *stats) {
    uint8_t bytes[4];
    if (!read_bytes(file, bytes, sizeof(bytes), stats))
        return false;
    *value = (uint32_t)bytes[0] |
             ((uint32_t)bytes[1] << 8u) |
             ((uint32_t)bytes[2] << 16u) |
             ((uint32_t)bytes[3] << 24u);
    return true;
}

static bool read_u64(FILE *file, uint64_t *value,
                     Dk1SceneAssetCacheIoStats *stats) {
    uint8_t bytes[8];
    size_t i;
    uint64_t result = 0u;
    if (!read_bytes(file, bytes, sizeof(bytes), stats))
        return false;
    for (i = 0u; i < sizeof(bytes); ++i)
        result |= (uint64_t)bytes[i] << (i * 8u);
    *value = result;
    return true;
}

static bool write_size(FILE *file, size_t value,
                       Dk1SceneAssetCacheIoStats *stats) {
    return write_u64(file, (uint64_t)value, stats);
}

static bool read_size(FILE *file, size_t *value,
                      Dk1SceneAssetCacheIoStats *stats) {
    uint64_t stored = 0u;
    if (!read_u64(file, &stored, stats) || stored > (uint64_t)SIZE_MAX)
        return false;
    *value = (size_t)stored;
    return true;
}

bool dk1_scene_asset_cache_key(const Dk1RomImage *rom,
                               uint16_t level_id,
                               bool special_assets,
                               bool state_package,
                               Dk1SceneAssetCacheKey *key) {
    if (rom == NULL || rom->data == NULL || rom->size == 0u || key == NULL)
        return false;
    key->rom_signature = hash_bytes(rom->data, rom->size);
    key->rom_size = (uint64_t)rom->size;
    key->level_id = level_id;
    key->options = (uint8_t)((special_assets ? 1u : 0u) |
                             (state_package ? 2u : 0u));
    return true;
}

bool dk1_scene_asset_cache_default_path(const char *rom_path,
                                        uint16_t level_id,
                                        bool special_assets,
                                        bool state_package,
                                        char *output,
                                        size_t capacity) {
    int written;
    if (rom_path == NULL || rom_path[0] == '\0' ||
        output == NULL || capacity == 0u)
        return false;
    written = snprintf(output, capacity,
                       "%s.dk1-assets-%04X-%u%u.bin",
                       rom_path, (unsigned)level_id,
                       special_assets ? 1u : 0u,
                       state_package ? 1u : 0u);
    return written >= 0 && (size_t)written < capacity;
}

bool dk1_scene_asset_cache_write(const char *path,
                                 const Dk1SceneAssetCacheKey *key,
                                 const Dk1SceneMemory *scene,
                                 Dk1SceneAssetCacheIoStats *stats) {
    Dk1SceneAssetCacheIoStats local_stats;
    Dk1SceneAssetCacheIoStats *output = stats != NULL ? stats : &local_stats;
    uint8_t palette_bytes[DK1_SCENE_ASSET_CACHE_PALETTE_BYTES];
    uint64_t vram_signature;
    uint64_t palette_signature;
    uint64_t payload_signature;
    size_t nonzero_vram;
    size_t nonzero_colors;
    char temporary[DK1_SCENE_ASSET_CACHE_PATH_CAPACITY + 8u];
    FILE *file;
    int path_length;
    bool ok;

    memset(output, 0, sizeof(*output));
    if (path == NULL || path[0] == '\0' || key == NULL || scene == NULL ||
        scene->assets.package_count != scene->packages.count ||
        scene->assets.palette_uploads != scene->recipe.palette_upload_count)
        return false;

    palette_to_bytes(&scene->cgram, palette_bytes);
    vram_signature = hash_bytes(scene->vram.bytes,
                                sizeof(scene->vram.bytes));
    palette_signature = hash_bytes(palette_bytes, sizeof(palette_bytes));
    payload_signature = hash_continue(scene->vram.bytes,
                                      sizeof(scene->vram.bytes), 0u);
    payload_signature = hash_continue(palette_bytes, sizeof(palette_bytes),
                                      payload_signature);
    nonzero_vram = count_nonzero_bytes(scene->vram.bytes,
                                      sizeof(scene->vram.bytes));
    nonzero_colors = count_nonzero_colors(&scene->cgram);

    path_length = snprintf(temporary, sizeof(temporary), "%s.tmp", path);
    if (path_length < 0 || (size_t)path_length >= sizeof(temporary))
        return false;
    file = fopen(temporary, "wb");
    if (file == NULL)
        return false;

    ok = write_bytes(file, dk1_scene_asset_cache_magic,
                     sizeof(dk1_scene_asset_cache_magic), output) &&
         write_u32(file, DK1_SCENE_ASSET_CACHE_FORMAT_VERSION, output) &&
         write_u16(file, key->level_id, output) &&
         write_u8(file, key->options, output) &&
         write_u8(file, 0u, output) &&
         write_u64(file, key->rom_size, output) &&
         write_u64(file, key->rom_signature, output) &&
         write_u32(file, DK1_SCENE_ASSET_CACHE_VRAM_SIZE, output) &&
         write_u32(file, DK1_SCENE_ASSET_CACHE_PALETTE_BYTES, output) &&
         write_u64(file, payload_signature, output) &&
         write_u64(file, vram_signature, output) &&
         write_u64(file, palette_signature, output) &&
         write_size(file, scene->assets.package_count, output) &&
         write_size(file, scene->assets.package_records, output) &&
         write_size(file, scene->assets.direct_records, output) &&
         write_size(file, scene->assets.compressed_records, output) &&
         write_size(file, scene->assets.dma_bytes, output) &&
         write_size(file, scene->assets.decompressed_bytes, output) &&
         write_size(file, scene->assets.palette_uploads, output) &&
         write_size(file, scene->assets.palette_colors, output) &&
         write_size(file, nonzero_vram, output) &&
         write_size(file, nonzero_colors, output) &&
         write_bytes(file, scene->vram.bytes,
                     sizeof(scene->vram.bytes), output) &&
         write_bytes(file, palette_bytes, sizeof(palette_bytes), output);
    if (fclose(file) != 0)
        ok = false;
    if (!ok) {
        (void)remove(temporary);
        return false;
    }
    (void)remove(path);
    if (rename(temporary, path) != 0) {
        (void)remove(temporary);
        return false;
    }
    return true;
}

bool dk1_scene_asset_cache_read(const char *path,
                                const Dk1SceneAssetCacheKey *key,
                                Dk1SceneMemory *scene,
                                Dk1SceneAssetCacheIoStats *stats) {
    Dk1SceneAssetCacheIoStats local_stats;
    Dk1SceneAssetCacheIoStats *output = stats != NULL ? stats : &local_stats;
    uint8_t magic[DK1_SCENE_ASSET_CACHE_MAGIC_SIZE];
    uint8_t options = 0u;
    uint8_t reserved = 0u;
    uint8_t palette_bytes[DK1_SCENE_ASSET_CACHE_PALETTE_BYTES];
    uint8_t *vram_bytes;
    uint16_t level_id = 0u;
    uint32_t version = 0u;
    uint32_t vram_size = 0u;
    uint32_t palette_size = 0u;
    uint64_t rom_size = 0u;
    uint64_t rom_signature = 0u;
    uint64_t stored_payload_signature = 0u;
    uint64_t stored_vram_signature = 0u;
    uint64_t stored_palette_signature = 0u;
    uint64_t actual_payload_signature;
    uint64_t actual_vram_signature;
    uint64_t actual_palette_signature;
    Dk1SceneAssetStats restored;
    FILE *file;
    bool ok;

    memset(output, 0, sizeof(*output));
    memset(&restored, 0, sizeof(restored));
    if (path == NULL || path[0] == '\0' || key == NULL || scene == NULL)
        return false;
    file = fopen(path, "rb");
    if (file == NULL)
        return false;
    output->file_found = true;
    vram_bytes = (uint8_t *)malloc(DK1_SCENE_ASSET_CACHE_VRAM_SIZE);
    if (vram_bytes == NULL) {
        fclose(file);
        output->rejected = true;
        return false;
    }

    ok = read_bytes(file, magic, sizeof(magic), output) &&
         read_u32(file, &version, output) &&
         read_u16(file, &level_id, output) &&
         read_u8(file, &options, output) &&
         read_u8(file, &reserved, output) &&
         read_u64(file, &rom_size, output) &&
         read_u64(file, &rom_signature, output) &&
         read_u32(file, &vram_size, output) &&
         read_u32(file, &palette_size, output) &&
         read_u64(file, &stored_payload_signature, output) &&
         read_u64(file, &stored_vram_signature, output) &&
         read_u64(file, &stored_palette_signature, output) &&
         read_size(file, &restored.package_count, output) &&
         read_size(file, &restored.package_records, output) &&
         read_size(file, &restored.direct_records, output) &&
         read_size(file, &restored.compressed_records, output) &&
         read_size(file, &restored.dma_bytes, output) &&
         read_size(file, &restored.decompressed_bytes, output) &&
         read_size(file, &restored.palette_uploads, output) &&
         read_size(file, &restored.palette_colors, output) &&
         read_size(file, &restored.nonzero_vram_bytes, output) &&
         read_size(file, &restored.nonzero_palette_colors, output) &&
         read_bytes(file, vram_bytes,
                    DK1_SCENE_ASSET_CACHE_VRAM_SIZE, output) &&
         read_bytes(file, palette_bytes, sizeof(palette_bytes), output);
    if (ok) {
        const int trailing = fgetc(file);
        if (trailing != EOF || ferror(file) != 0)
            ok = false;
    }
    fclose(file);

    if (ok) {
        actual_vram_signature =
            hash_bytes(vram_bytes, DK1_SCENE_ASSET_CACHE_VRAM_SIZE);
        actual_palette_signature =
            hash_bytes(palette_bytes, sizeof(palette_bytes));
        actual_payload_signature =
            hash_continue(vram_bytes, DK1_SCENE_ASSET_CACHE_VRAM_SIZE, 0u);
        actual_payload_signature =
            hash_continue(palette_bytes, sizeof(palette_bytes),
                          actual_payload_signature);
        ok = memcmp(magic, dk1_scene_asset_cache_magic,
                    sizeof(magic)) == 0 &&
             version == DK1_SCENE_ASSET_CACHE_FORMAT_VERSION &&
             level_id == key->level_id &&
             options == key->options &&
             reserved == 0u &&
             rom_size == key->rom_size &&
             rom_signature == key->rom_signature &&
             vram_size == DK1_SCENE_ASSET_CACHE_VRAM_SIZE &&
             palette_size == DK1_SCENE_ASSET_CACHE_PALETTE_BYTES &&
             stored_payload_signature == actual_payload_signature &&
             stored_vram_signature == actual_vram_signature &&
             stored_palette_signature == actual_palette_signature &&
             restored.package_count == scene->packages.count &&
             restored.palette_uploads == scene->recipe.palette_upload_count &&
             restored.package_records ==
                 restored.direct_records + restored.compressed_records &&
             restored.nonzero_vram_bytes <= DK1_VRAM_BYTE_COUNT &&
             restored.nonzero_palette_colors <= DK1_CGRAM_COLOR_COUNT &&
             restored.nonzero_vram_bytes ==
                 count_nonzero_bytes(vram_bytes,
                                     DK1_SCENE_ASSET_CACHE_VRAM_SIZE);
    }
    if (!ok) {
        free(vram_bytes);
        output->rejected = true;
        return false;
    }

    dk1_vram_image_init(&scene->vram);
    memcpy(scene->vram.bytes, vram_bytes,
           DK1_SCENE_ASSET_CACHE_VRAM_SIZE);
    palette_from_bytes(palette_bytes, &scene->cgram);
    restored.vram_signature = actual_vram_signature;
    restored.palette_signature = actual_palette_signature;
    restored.textures_loaded =
        restored.package_count != 0u &&
        restored.package_records != 0u &&
        restored.nonzero_vram_bytes != 0u;
    restored.palettes_loaded =
        restored.palette_uploads != 0u &&
        restored.nonzero_palette_colors != 0u;
    scene->assets = restored;
    free(vram_bytes);
    return true;
}

bool dk1_scene_memory_load_cached(const Dk1RomImage *rom,
                                  uint16_t level_id,
                                  bool special_assets,
                                  bool state_package,
                                  const char *cache_path,
                                  Dk1SceneMemory *scene) {
    Dk1SceneAssetCacheKey key;
    Dk1SceneAssetCacheIoStats read_stats;
    Dk1SceneAssetCacheIoStats write_stats;
    bool cache_enabled;

    if (!dk1_scene_memory_prepare(rom, level_id,
                                  special_assets, state_package, scene))
        return false;
    cache_enabled = cache_path != NULL && cache_path[0] != '\0';
    if (!cache_enabled)
        return dk1_scene_memory_load_assets(rom, scene);
    if (!dk1_scene_asset_cache_key(rom, level_id,
                                   special_assets, state_package, &key))
        return false;

    scene->cache.attempted = true;
    scene->cache.rom_signature = key.rom_signature;
    if (dk1_scene_asset_cache_read(cache_path, &key, scene, &read_stats)) {
        scene->cache.file_found = true;
        scene->cache.hit = true;
        scene->cache.bytes_read = read_stats.bytes_read;
        return true;
    }

    scene->cache.file_found = read_stats.file_found;
    scene->cache.rejected = read_stats.rejected;
    scene->cache.bytes_read = read_stats.bytes_read;
    if (!dk1_scene_memory_load_assets(rom, scene))
        return false;
    if (dk1_scene_asset_cache_write(cache_path, &key, scene, &write_stats)) {
        scene->cache.written = true;
        scene->cache.bytes_written = write_stats.bytes_written;
    }
    return true;
}
