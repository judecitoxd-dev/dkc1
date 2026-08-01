#include "dk1/scene_signature.h"

uint64_t dk1_fnv1a64(const void *data, size_t size, uint64_t hash) {
    const unsigned char *bytes = (const unsigned char *)data;
    size_t i;
    if (hash == 0u)
        hash = UINT64_C(1469598103934665603);
    for (i = 0u; i < size; ++i) {
        hash ^= bytes[i];
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

uint64_t dk1_scene_memory_signature(const Dk1SceneMemory *scene) {
    uint64_t hash = 0u;
    if (scene == NULL)
        return 0u;
    hash = dk1_fnv1a64(&scene->recipe, sizeof(scene->recipe), hash);
    hash = dk1_fnv1a64(&scene->packages, sizeof(scene->packages), hash);
    hash = dk1_fnv1a64(&scene->ppu, sizeof(scene->ppu), hash);
    hash = dk1_fnv1a64(&scene->terrain, sizeof(scene->terrain), hash);
    hash = dk1_fnv1a64(&scene->camera, sizeof(scene->camera), hash);
    hash = dk1_fnv1a64(scene->vram.bytes, sizeof(scene->vram.bytes), hash);
    hash = dk1_fnv1a64(scene->cgram.colors,
                       sizeof(scene->cgram.colors), hash);
    return dk1_fnv1a64(&scene->assets, sizeof(scene->assets), hash);
}
