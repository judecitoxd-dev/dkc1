#ifndef DK1_SCENE_RUNTIME_H
#define DK1_SCENE_RUNTIME_H
#include <stdbool.h>
#include <stdint.h>
#include "dk1/scene_view.h"
#define DK1_RUNTIME_LEFT 0x0200u
#define DK1_RUNTIME_RIGHT 0x0100u
#define DK1_RUNTIME_UP 0x0800u
#define DK1_RUNTIME_DOWN 0x0400u
typedef struct Dk1SceneRuntime { Dk1SceneView view; uint64_t frame; } Dk1SceneRuntime;
bool dk1_scene_runtime_step(const Dk1SceneMemory *scene,uint16_t held,Dk1SceneRuntime *runtime);
#endif
