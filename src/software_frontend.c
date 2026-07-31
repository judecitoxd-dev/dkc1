#include "dk1/software_frontend.h"
#include <stdlib.h>
#include <string.h>
#include "dk1/scene_signature.h"

static void update_marker(Dk1SoftwareFrontend *frontend) {
    Dk1OamSprite sprite;
    sprite.x = frontend->marker_x;
    sprite.y = (uint8_t)frontend->marker_y;
    sprite.tile_number = frontend->marker_tile;
    sprite.palette = frontend->marker_palette;
    sprite.priority = 3u;
    sprite.horizontal_flip = false;
    sprite.vertical_flip = false;
    sprite.large = false;
    sprite.width = 8u;
    sprite.height = 8u;
    (void)dk1_oam_encode_sprite(&frontend->oam, frontend->obsel, 0u, &sprite);
}

bool dk1_software_frontend_init(const Dk1SceneMemory *scene,
                                uint16_t width, uint16_t height,
                                Dk1SoftwareFrontend *frontend) {
    uint8_t obsel = 0u;
    Dk1DynamicStreamUpdate update;
    if (scene == NULL || frontend == NULL || width == 0u || height == 0u) return false;
    memset(frontend, 0, sizeof(*frontend));
    frontend->runtime.view.width = width;
    frontend->runtime.view.height = height;
    frontend->marker_x = (int16_t)(width / 2u);
    frontend->marker_y = (int16_t)(height / 2u);
    frontend->marker_tile = 0u;
    frontend->marker_palette = 0u;
    (void)dk1_ppu_preset_register(&scene->ppu, 0x2101u, &obsel);
    frontend->obsel = obsel;
    dk1_oam_init(&frontend->oam);
    update_marker(frontend);
    if (!dk1_scene_view_clamp(scene, &frontend->runtime.view)) return false;
    return dk1_dynamic_stream_update(frontend->runtime.view, 128u,
                                     &frontend->stream, &update);
}

bool dk1_software_frontend_step(const Dk1SceneMemory *scene, uint16_t held,
                                Dk1SoftwareFrontend *frontend) {
    int32_t x, y;
    Dk1DynamicStreamUpdate update;
    if (scene == NULL || frontend == NULL) return false;
    dk1_host_input_update(&frontend->input, held);
    if (!dk1_scene_runtime_step(scene, held, &frontend->runtime)) return false;
    if (!dk1_dynamic_stream_update(frontend->runtime.view, 128u,
                                   &frontend->stream, &update)) return false;
    x = frontend->marker_x;
    y = frontend->marker_y;
    if (held & DK1_HOST_BUTTON_L) x -= 2;
    if (held & DK1_HOST_BUTTON_R) x += 2;
    if (held & DK1_HOST_BUTTON_Y) y -= 2;
    if (held & DK1_HOST_BUTTON_B) y += 2;
    if (x < -8) x = -8;
    if (x > (int32_t)frontend->runtime.view.width) x = frontend->runtime.view.width;
    if (y < 0) y = 0;
    if (y > 255) y = 255;
    frontend->marker_x = (int16_t)x;
    frontend->marker_y = (int16_t)y;
    update_marker(frontend);
    return true;
}

bool dk1_software_frontend_render(const Dk1RomImage *rom,
                                  const Dk1SceneMemory *scene,
                                  Dk1SoftwareFrontend *frontend,
                                  Dk1RgbaSurface destination) {
    Dk1Rgba8 *stream_pixels = NULL;
    Dk1RgbaSurface stream;
    uint8_t inidisp = 15u;
    bool streamed = false;
    size_t i, count;
    if (scene == NULL || frontend == NULL || destination.pixels == NULL ||
        destination.width != frontend->runtime.view.width ||
        destination.height != frontend->runtime.view.height) return false;
    if (!dk1_ppu_compose_mode1(scene,
        frontend->runtime.view.camera_x / 2u, frontend->runtime.view.camera_y / 2u,
        0u, frontend->runtime.view.camera_y, destination)) return false;
    if (rom != NULL) {
        stream_pixels = (Dk1Rgba8 *)calloc(destination.width * destination.height, sizeof(*stream_pixels));
        if (stream_pixels != NULL) {
            stream = (Dk1RgbaSurface){stream_pixels, destination.width, destination.height, destination.width};
            streamed = dk1_scene_render_bg1(rom, scene, frontend->runtime.view, stream);
            if (streamed) memcpy(destination.pixels, stream_pixels,
                                 destination.width * destination.height * sizeof(*stream_pixels));
            free(stream_pixels);
        }
    }
    if (!dk1_oam_render(&frontend->oam, frontend->obsel, &scene->vram, &scene->cgram,
                        0u, 3u, destination)) return false;
    if (dk1_ppu_preset_register(&scene->ppu, 0x2100u, &inidisp)) inidisp &= 0x0Fu;
    count = destination.width * destination.height;
    if (inidisp < 15u) {
        for (i = 0; i < count; ++i) {
            uint16_t r = destination.pixels[i].r;
            uint16_t g = destination.pixels[i].g;
            uint16_t b = destination.pixels[i].b;
            destination.pixels[i].r = (uint8_t)((r * inidisp + 7u) / 15u);
            destination.pixels[i].g = (uint8_t)((g * inidisp + 7u) / 15u);
            destination.pixels[i].b = (uint8_t)((b * inidisp + 7u) / 15u);
        }
    }
    (void)streamed;
    return true;
}

uint64_t dk1_software_frontend_signature(const Dk1SoftwareFrontend *frontend) {
    uint64_t hash = 0;
    if (frontend == NULL) return 0;
    hash = dk1_fnv1a64(&frontend->runtime, sizeof(frontend->runtime), hash);
    hash = dk1_fnv1a64(&frontend->input, sizeof(frontend->input), hash);
    hash = dk1_fnv1a64(&frontend->stream, sizeof(frontend->stream), hash);
    hash = dk1_fnv1a64(&frontend->marker_x, sizeof(frontend->marker_x), hash);
    hash = dk1_fnv1a64(&frontend->marker_y, sizeof(frontend->marker_y), hash);
    return dk1_fnv1a64(&frontend->oam, sizeof(frontend->oam), hash);
}
