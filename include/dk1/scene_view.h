#ifndef DK1_SCENE_VIEW_H
#define DK1_SCENE_VIEW_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/scene_memory.h"
#include "dk1/bg_renderer.h"
typedef struct Dk1SceneView { uint16_t camera_x; uint16_t camera_y; uint16_t width; uint16_t height; } Dk1SceneView;
bool dk1_scene_view_clamp(const Dk1SceneMemory *scene,Dk1SceneView *view);
bool dk1_scene_render_bg1(const Dk1RomImage *rom,const Dk1SceneMemory *scene,Dk1SceneView view,Dk1RgbaSurface destination);
#endif
