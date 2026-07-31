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
    dk1_oam_init(&frontend->oam);
    (void)dk1_oam_encode_sprite(&frontend->oam, frontend->obsel, 0u, &sprite);
    frontend->player_visual_ready = false;
}

static void update_preview_screen_position(Dk1SoftwareFrontend *frontend) {
    int32_t x = (int32_t)frontend->player_preview.motion.world_x -
                frontend->runtime.view.camera_x;
    int32_t y = 224 - (int32_t)frontend->runtime.view.camera_y -
                frontend->player_preview.motion.world_y;
    if (x < -256) x = -256;
    if (x > 255) x = 255;
    if (y < -128) y = -128;
    if (y > 255) y = 255;
    frontend->marker_x = (int16_t)x;
    frontend->marker_y = (int16_t)y;
    frontend->player_frame = frontend->player_preview.frame_id;
}

static bool update_player_visual(const Dk1RomImage *rom,
                                 const Dk1SceneMemory *scene,
                                 Dk1SoftwareFrontend *frontend) {
    Dk1ObjectScreenTransform transform;
    Dk1ObjectFrameOamStyle style;
    if (rom == NULL) {
        update_marker(frontend);
        return true;
    }
    transform.world_x = frontend->player_preview.motion.world_x;
    transform.world_y = frontend->player_preview.motion.world_y;
    transform.camera_x = (int16_t)frontend->runtime.view.camera_x;
    transform.vertical_origin = 224;
    transform.vertical_scroll = (int16_t)frontend->runtime.view.camera_y;
    transform.vertical_offset = 0;
    transform.object_flags = frontend->player_preview.facing_left ? 0x4000u : 0u;
    style.base_tile_number = frontend->player_tile_base;
    style.palette = frontend->marker_palette;
    style.priority = 3u;
    style.horizontal_flip = false;
    style.vertical_flip = false;
    frontend->player_visual_ready = dk1_player_visual_build(
        rom, &scene->vram, frontend->player_frame, transform, style,
        frontend->obsel, &frontend->oam, &frontend->player_vram,
        &frontend->player_visual);
    if (!frontend->player_visual_ready) update_marker(frontend);
    return true;
}

bool dk1_software_frontend_init(const Dk1SceneMemory *scene,
                                uint16_t width, uint16_t height,
                                Dk1SoftwareFrontend *frontend) {
    uint8_t obsel = 0u;
    Dk1DynamicStreamUpdate update;
    int16_t start_x;
    int16_t start_y;
    if (scene == NULL || frontend == NULL || width == 0u || height == 0u) return false;
    memset(frontend, 0, sizeof(*frontend));
    frontend->runtime.view.width = width;
    frontend->runtime.view.height = height;
    frontend->marker_x = (int16_t)(width / 2u);
    frontend->marker_y = (int16_t)(height / 2u);
    frontend->marker_tile = 0u;
    frontend->marker_palette = 0u;
    frontend->player_frame = 0x0330u;
    frontend->player_tile_base = 0x0040u;
    frontend->player_vram = scene->vram;
    (void)dk1_ppu_preset_register(&scene->ppu, 0x2101u, &obsel);
    frontend->obsel = obsel;
    if (!dk1_scene_view_clamp(scene, &frontend->runtime.view)) return false;
    start_x = (int16_t)(frontend->runtime.view.camera_x + width / 2u);
    start_y = (int16_t)(224 - frontend->runtime.view.camera_y - height / 2u);
    dk1_player_preview_init(&frontend->player_preview, start_x, start_y);
    update_preview_screen_position(frontend);
    update_marker(frontend);
    return dk1_dynamic_stream_update(frontend->runtime.view, 128u,
                                     &frontend->stream, &update);
}

bool dk1_software_frontend_step(const Dk1SceneMemory *scene, uint16_t held,
                                Dk1SoftwareFrontend *frontend) {
    Dk1DynamicStreamUpdate update;
    int32_t maximum_world_x;
    if (scene == NULL || frontend == NULL) return false;
    dk1_host_input_update(&frontend->input, held);
    if (!dk1_scene_runtime_step(scene, held, &frontend->runtime)) return false;
    if (!dk1_dynamic_stream_update(frontend->runtime.view, 128u,
                                   &frontend->stream, &update)) return false;
    dk1_player_preview_step(&frontend->player_preview, held, frontend->input.pressed);
    maximum_world_x = (int32_t)scene->camera.maximum_x + frontend->runtime.view.width + 32;
    if (frontend->player_preview.motion.world_x < -32)
        frontend->player_preview.motion.world_x = -32;
    if (frontend->player_preview.motion.world_x > maximum_world_x)
        frontend->player_preview.motion.world_x = (int16_t)maximum_world_x;
    update_preview_screen_position(frontend);
    frontend->player_visual_ready = false;
    return true;
}

bool dk1_software_frontend_render(const Dk1RomImage *rom,
                                  const Dk1SceneMemory *scene,
                                  Dk1SoftwareFrontend *frontend,
                                  Dk1RgbaSurface destination) {
    Dk1Rgba8 *stream_pixels = NULL;
    Dk1RgbaSurface stream;
    const Dk1VramImage *sprite_vram;
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
    if (!update_player_visual(rom, scene, frontend)) return false;
    sprite_vram = frontend->player_visual_ready ? &frontend->player_vram : &scene->vram;
    if (!dk1_oam_render(&frontend->oam, frontend->obsel, sprite_vram, &scene->cgram,
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
    hash = dk1_fnv1a64(&frontend->player_preview, sizeof(frontend->player_preview), hash);
    hash = dk1_fnv1a64(&frontend->marker_x, sizeof(frontend->marker_x), hash);
    hash = dk1_fnv1a64(&frontend->marker_y, sizeof(frontend->marker_y), hash);
    hash = dk1_fnv1a64(&frontend->player_frame, sizeof(frontend->player_frame), hash);
    hash = dk1_fnv1a64(&frontend->player_visual, sizeof(frontend->player_visual), hash);
    return dk1_fnv1a64(&frontend->oam, sizeof(frontend->oam), hash);
}
