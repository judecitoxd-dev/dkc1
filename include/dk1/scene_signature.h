#ifndef DK1_SCENE_SIGNATURE_H
#define DK1_SCENE_SIGNATURE_H
#include <stddef.h>
#include <stdint.h>
#include "dk1/scene_memory.h"
uint64_t dk1_fnv1a64(const void *data,size_t size,uint64_t seed);
uint64_t dk1_scene_memory_signature(const Dk1SceneMemory *scene);
#endif
