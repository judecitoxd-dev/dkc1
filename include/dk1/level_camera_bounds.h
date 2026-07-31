#ifndef DK1_LEVEL_CAMERA_BOUNDS_H
#define DK1_LEVEL_CAMERA_BOUNDS_H
#include <stdbool.h>
#include <stdint.h>
#include "dk1/rom_image.h"
typedef struct Dk1LevelCameraBounds { uint16_t minimum_x; uint16_t maximum_x; } Dk1LevelCameraBounds;
bool dk1_level_camera_bounds_read(const Dk1RomImage *rom,uint16_t level_id,Dk1LevelCameraBounds *bounds);
#endif
