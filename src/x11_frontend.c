#define _POSIX_C_SOURCE 200809L
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "dk1/level_software_frontend.h"
#include "dk1/rom_image.h"
#include "dk1/scene_memory.h"

static uint16_t key_button(KeySym key) {
    switch (key) {
    case XK_Left: case XK_a: case XK_A: return DK1_HOST_BUTTON_LEFT;
    case XK_Right: case XK_d: case XK_D: return DK1_HOST_BUTTON_RIGHT;
    case XK_Up: case XK_w: case XK_W: return DK1_HOST_BUTTON_UP;
    case XK_Down: case XK_s: case XK_S: return DK1_HOST_BUTTON_DOWN;
    case XK_z: case XK_Z: return DK1_HOST_BUTTON_B;
    case XK_x: case XK_X: return DK1_HOST_BUTTON_A;
    case XK_q: case XK_Q: return DK1_HOST_BUTTON_L;
    case XK_e: case XK_E: return DK1_HOST_BUTTON_R;
    case XK_u: case XK_U: return DK1_HOST_BUTTON_Y;
    case XK_i: case XK_I: return DK1_HOST_BUTTON_X;
    case XK_Return: return DK1_HOST_BUTTON_START;
    case XK_Shift_L: case XK_Shift_R: return DK1_HOST_BUTTON_SELECT;
    default: return 0u;
    }
}

int main(int argc, char **argv) {
    Dk1OwnedRom owned = {0};
    Dk1RomImage rom = {0};
    Dk1RomIdentity identity;
    Dk1SceneMemory *scene = NULL;
    Dk1SoftwareFrontend frontend;
    Dk1LevelSoftwareFrontendStats source_stats;
    Dk1Rgba8 *pixels = NULL;
    Dk1RgbaSurface surface;
    Display *display = NULL;
    Window window;
    GC gc = 0;
    XImage *image = NULL;
    Atom wm_delete;
    uint16_t held = 0u;
    unsigned long level;
    int width = 384;
    int height = 224;
    int running = 1;
    int result = 1;
    struct timespec delay = {0, 16666667L};

    if (argc < 3 || argc > 5) {
        fprintf(stderr, "usage: %s ROM LEVEL [WIDTH HEIGHT]\n", argv[0]);
        return 2;
    }
    level = strtoul(argv[2], NULL, 0);
    if (argc >= 5) {
        width = atoi(argv[3]);
        height = atoi(argv[4]);
    }
    if (level >= DK1_SCENE_LEVEL_COUNT || width < 256 || height < 192 ||
        width > 1024 || height > 512)
        return 2;
    if (!dk1_rom_load_file(argv[1], &owned, &rom) ||
        !dk1_rom_identity(&rom, &identity) ||
        !dk1_rom_identity_is_supported_rev2(&identity))
        goto cleanup;
    scene = (Dk1SceneMemory *)malloc(sizeof(*scene));
    pixels = (Dk1Rgba8 *)calloc((size_t)width * (size_t)height,
                                sizeof(*pixels));
    if (scene == NULL || pixels == NULL ||
        !dk1_scene_memory_load(&rom, (uint16_t)level, false, false, scene) ||
        !dk1_level_software_frontend_init(&rom, scene, (uint16_t)level,
                                          (uint16_t)width, (uint16_t)height,
                                          &frontend, &source_stats))
        goto cleanup;

    if (source_stats.barrel_spawned) {
        fprintf(stderr,
                "level object: Barrel record=%zu source=$%06X x=$%04X y=$%04X "
                "callback=$%06X\n",
                source_stats.barrel_spawn.record_index,
                (unsigned)source_stats.barrel_spawn.record.source_pc,
                source_stats.barrel_spawn.record.world_x,
                (unsigned)(uint16_t)source_stats.barrel_spawn.record.world_y,
                (unsigned)source_stats.barrel_spawn.callback_pc);
    }

    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fputs("could not open X11 display\n", stderr);
        goto cleanup;
    }
    window = XCreateSimpleWindow(display, DefaultRootWindow(display),
                                 0, 0, (unsigned)width, (unsigned)height,
                                 0, 0, 0);
    XStoreName(display, window, "DK1 clean-room scene preview");
    XSelectInput(display, window,
                 ExposureMask | KeyPressMask | KeyReleaseMask |
                 StructureNotifyMask);
    wm_delete = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wm_delete, 1);
    gc = XCreateGC(display, window, 0, NULL);
    XMapWindow(display, window);
    image = XCreateImage(display,
                         DefaultVisual(display, DefaultScreen(display)),
                         (unsigned)DefaultDepth(display, DefaultScreen(display)),
                         ZPixmap, 0,
                         (char *)calloc((size_t)width * (size_t)height, 4u),
                         (unsigned)width, (unsigned)height, 32, 0);
    if (image == NULL || image->data == NULL) goto cleanup;
    surface = (Dk1RgbaSurface){pixels, (size_t)width, (size_t)height,
                               (size_t)width};

    while (running) {
        while (XPending(display)) {
            XEvent event;
            XNextEvent(display, &event);
            if (event.type == ClientMessage &&
                (Atom)event.xclient.data.l[0] == wm_delete) {
                running = 0;
            } else if (event.type == KeyPress || event.type == KeyRelease) {
                KeySym key = XLookupKeysym(&event.xkey, 0);
                uint16_t button = key_button(key);
                if (event.type == KeyPress && key == XK_Escape) running = 0;
                if (event.type == KeyPress) held |= button;
                else held = (uint16_t)(held & (uint16_t)~button);
            }
        }
        if (!dk1_software_frontend_step(scene, held, &frontend) ||
            !dk1_software_frontend_render(&rom, scene, &frontend, surface))
            break;
        {
            int y;
            int x;
            uint32_t *destination = (uint32_t *)image->data;
            for (y = 0; y < height; ++y) {
                for (x = 0; x < width; ++x) {
                    Dk1Rgba8 color =
                        pixels[(size_t)y * (size_t)width + (size_t)x];
                    destination[(size_t)y * (size_t)width + (size_t)x] =
                        ((uint32_t)color.r << 16u) |
                        ((uint32_t)color.g << 8u) | color.b;
                }
            }
        }
        XPutImage(display, window, gc, image, 0, 0, 0, 0,
                  (unsigned)width, (unsigned)height);
        XFlush(display);
        nanosleep(&delay, NULL);
    }
    result = 0;

cleanup:
    if (image != NULL) XDestroyImage(image);
    if (display != NULL) {
        if (gc != 0) XFreeGC(display, gc);
        XCloseDisplay(display);
    }
    free(pixels);
    free(scene);
    dk1_rom_free(&owned);
    return result;
}
