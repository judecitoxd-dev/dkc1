#ifndef DK1_SOFTWARE_FRONTEND_H
#define DK1_SOFTWARE_FRONTEND_H

#include <stdbool.h>
#include <stdint.h>
#include "dk1/host_input.h"
#include "dk1/dynamic_stream.h"
#include "dk1/oam_image.h"
#include "dk1/ppu_compositor.h"
#include "dk1/scene_runtime.h"

typedef struct Dk1SoftwareFrontend {
    Dk1SceneRuntime runtime;
    Dk1HostInputState input;
    Dk1OamImage oam;
    Dk1DynamicStreamState stream;
    uint8_t obsel;
    int16_t marker_x;
    int16_t marker_y;
    uint16_t marker_tile;
    uint8_t marker_palette;
} Dk1SoftwareFrontend;

bool dk1_software_frontend_init(const Dk1SceneMemory *scene,
                                uint16_t width, uint16_t height,
                                Dk1SoftwareFrontend *frontend);
bool dk1_software_frontend_step(const Dk1SceneMemory *scene, uint16_t held,
                                Dk1SoftwareFrontend *frontend);
bool dk1_software_frontend_render(const Dk1RomImage *rom,
                                  const Dk1SceneMemory *scene,
                                  Dk1SoftwareFrontend *frontend,
                                  Dk1RgbaSurface destination);
uint64_t dk1_software_frontend_signature(const Dk1SoftwareFrontend *frontend);

#endif
