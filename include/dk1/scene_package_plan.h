#ifndef DK1_SCENE_PACKAGE_PLAN_H
#define DK1_SCENE_PACKAGE_PLAN_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#define DK1_SCENE_PACKAGE_PLAN_MAX 5u
typedef struct Dk1ScenePackagePlan { uint8_t ids[DK1_SCENE_PACKAGE_PLAN_MAX]; size_t count; } Dk1ScenePackagePlan;
bool dk1_scene_package_plan(uint16_t level_id,uint8_t primary,bool special_assets,bool include_state_package,Dk1ScenePackagePlan*plan);
#endif
