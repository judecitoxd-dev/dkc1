#ifndef DK1_APU_BOOT_IMAGE_H
#define DK1_APU_BOOT_IMAGE_H
#include <stdbool.h>
#include <stdint.h>
#include "dk1/rom_image.h"
#define DK1_APU_BOOT_IMAGE_SOURCE 0x8AA342u
#define DK1_APU_BOOT_IMAGE_SIZE 40u
typedef struct Dk1ApuBootImage { uint8_t bytes[DK1_APU_BOOT_IMAGE_SIZE]; uint64_t signature; } Dk1ApuBootImage;
bool dk1_apu_boot_image_read(const Dk1RomImage *rom,Dk1ApuBootImage *image);
#endif
