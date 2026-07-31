#ifndef DK1_CGRAM_IMAGE_H
#define DK1_CGRAM_IMAGE_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "dk1/rom_image.h"
#include "dk1/snes_graphics.h"
#define DK1_CGRAM_COLOR_COUNT 256u
typedef struct Dk1CgramImage { uint16_t colors[DK1_CGRAM_COLOR_COUNT]; } Dk1CgramImage;
void dk1_cgram_init(Dk1CgramImage *image);
bool dk1_cgram_upload_b9(const Dk1RomImage *rom, uint16_t source_address,
                         uint16_t cgram_start, uint16_t x_count,
                         Dk1CgramImage *image);
void dk1_cgram_decode_rgba(const Dk1CgramImage *image, Dk1Rgba8 output[DK1_CGRAM_COLOR_COUNT]);
#endif
