#ifndef DK1_SCENE_ASSET_CACHE_H
#define DK1_SCENE_ASSET_CACHE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/scene_memory.h"

#define DK1_SCENE_ASSET_CACHE_FORMAT_VERSION 1u
#define DK1_SCENE_ASSET_CACHE_PATH_CAPACITY 1024u

typedef struct Dk1SceneAssetCacheKey {
    uint64_t rom_signature;
    uint64_t rom_size;
    uint16_t level_id;
    uint8_t options;
} Dk1SceneAssetCacheKey;

typedef struct Dk1SceneAssetCacheIoStats {
    size_t bytes_read;
    size_t bytes_written;
    bool file_found;
    bool rejected;
} Dk1SceneAssetCacheIoStats;

bool dk1_scene_asset_cache_key(const Dk1RomImage *rom,
                               uint16_t level_id,
                               bool special_assets,
                               bool state_package,
                               Dk1SceneAssetCacheKey *key);

bool dk1_scene_asset_cache_default_path(const char *rom_path,
                                        uint16_t level_id,
                                        bool special_assets,
                                        bool state_package,
                                        char *output,
                                        size_t capacity);

bool dk1_scene_asset_cache_read(const char *path,
                                const Dk1SceneAssetCacheKey *key,
                                Dk1SceneMemory *scene,
                                Dk1SceneAssetCacheIoStats *stats);

bool dk1_scene_asset_cache_write(const char *path,
                                 const Dk1SceneAssetCacheKey *key,
                                 const Dk1SceneMemory *scene,
                                 Dk1SceneAssetCacheIoStats *stats);

/* The legal ROM is still required and validated on every run. A cache hit only
 * skips repeated scene graphics decompression and palette copying. Invalid or
 * stale files are rebuilt from the supplied ROM and never trusted as a ROM
 * replacement. Cache write failure is non-fatal. */
bool dk1_scene_memory_load_cached(const Dk1RomImage *rom,
                                  uint16_t level_id,
                                  bool special_assets,
                                  bool state_package,
                                  const char *cache_path,
                                  Dk1SceneMemory *scene);

#endif
