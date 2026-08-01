#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dk1/level_software_frontend.h"
#include "dk1/rom_image.h"
#include "dk1/scene_memory.h"

#define DK1_DEMO_LEVEL 0x0016u
#define DK1_DEMO_WIDTH 384
#define DK1_DEMO_HEIGHT 224

typedef struct Dk1Win32App {
    Dk1OwnedRom owned;
    Dk1RomImage rom;
    Dk1SceneMemory *scene;
    Dk1SoftwareFrontend frontend;
    Dk1LevelSoftwareFrontendStats source_stats;
    Dk1Rgba8 *pixels;
    uint32_t *dib_pixels;
    BITMAPINFO bitmap_info;
    HWND window;
    uint16_t held;
    uint16_t level;
    int width;
    int height;
    bool running;
    bool ready;
    bool completed;
} Dk1Win32App;

static Dk1Win32App *g_app;

static uint16_t key_button(WPARAM key) {
    switch (key) {
    case VK_LEFT: case 'A': return DK1_HOST_BUTTON_LEFT;
    case VK_RIGHT: case 'D': return DK1_HOST_BUTTON_RIGHT;
    case VK_UP: case 'W': return DK1_HOST_BUTTON_UP;
    case VK_DOWN: case 'S': return DK1_HOST_BUTTON_DOWN;
    case 'Z': return DK1_HOST_BUTTON_B;
    case 'X': return DK1_HOST_BUTTON_A;
    case 'Q': return DK1_HOST_BUTTON_L;
    case 'E': return DK1_HOST_BUTTON_R;
    case 'U': return DK1_HOST_BUTTON_Y;
    case 'I': return DK1_HOST_BUTTON_X;
    case VK_RETURN: return DK1_HOST_BUTTON_START;
    case VK_SHIFT: return DK1_HOST_BUTTON_SELECT;
    default: return 0u;
    }
}

static void present(HWND window, HDC dc) {
    RECT client;
    int client_width;
    int client_height;
    int destination_width;
    int destination_height;
    int destination_x;
    int destination_y;
    HBRUSH black;
    Dk1Win32App *app = g_app;
    if (app == NULL || app->dib_pixels == NULL) return;
    GetClientRect(window, &client);
    client_width = client.right - client.left;
    client_height = client.bottom - client.top;
    if (client_width <= 0 || client_height <= 0) return;

    destination_width = client_width;
    destination_height = MulDiv(destination_width, app->height, app->width);
    if (destination_height > client_height) {
        destination_height = client_height;
        destination_width = MulDiv(destination_height, app->width, app->height);
    }
    destination_x = (client_width - destination_width) / 2;
    destination_y = (client_height - destination_height) / 2;

    black = (HBRUSH)GetStockObject(BLACK_BRUSH);
    FillRect(dc, &client, black);
    SetStretchBltMode(dc, COLORONCOLOR);
    StretchDIBits(dc,
                  destination_x, destination_y,
                  destination_width, destination_height,
                  0, 0, app->width, app->height,
                  app->dib_pixels, &app->bitmap_info,
                  DIB_RGB_COLORS, SRCCOPY);
}

static LRESULT CALLBACK window_proc(HWND window, UINT message,
                                    WPARAM wparam, LPARAM lparam) {
    switch (message) {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (wparam == VK_ESCAPE) {
            DestroyWindow(window);
            return 0;
        }
        if (g_app != NULL)
            g_app->held = (uint16_t)(g_app->held | key_button(wparam));
        return 0;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        if (g_app != NULL)
            g_app->held = (uint16_t)(g_app->held &
                (uint16_t)~key_button(wparam));
        return 0;
    case WM_KILLFOCUS:
        if (g_app != NULL) g_app->held = 0u;
        return 0;
    case WM_PAINT: {
        PAINTSTRUCT paint;
        HDC dc = BeginPaint(window, &paint);
        present(window, dc);
        EndPaint(window, &paint);
        return 0;
    }
    case WM_ERASEBKGND:
        return 1;
    case WM_CLOSE:
        DestroyWindow(window);
        return 0;
    case WM_DESTROY:
        if (g_app != NULL) g_app->running = false;
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProcA(window, message, wparam, lparam);
    }
}

static bool choose_rom(char path[MAX_PATH]) {
    OPENFILENAMEA dialog;
    memset(&dialog, 0, sizeof(dialog));
    path[0] = '\0';
    dialog.lStructSize = sizeof(dialog);
    dialog.hwndOwner = NULL;
    dialog.lpstrFilter =
        "SNES ROM (*.sfc;*.smc)\0*.sfc;*.smc\0All files (*.*)\0*.*\0\0";
    dialog.lpstrFile = path;
    dialog.nMaxFile = MAX_PATH;
    dialog.lpstrTitle = "Select your legal Donkey Kong Country Rev 2 ROM";
    dialog.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST |
                   OFN_HIDEREADONLY;
    return GetOpenFileNameA(&dialog) != FALSE;
}

static void convert_frame(Dk1Win32App *app) {
    size_t count;
    size_t i;
    if (app == NULL || app->pixels == NULL || app->dib_pixels == NULL) return;
    count = (size_t)app->width * (size_t)app->height;
    for (i = 0u; i < count; ++i) {
        Dk1Rgba8 color = app->pixels[i];
        app->dib_pixels[i] = ((uint32_t)color.r << 16u) |
                             ((uint32_t)color.g << 8u) |
                             (uint32_t)color.b;
    }
}

static bool initialize_game(Dk1Win32App *app, const char *rom_path) {
    Dk1RomIdentity identity;
    if (app == NULL || rom_path == NULL) return false;
    app->scene = (Dk1SceneMemory *)malloc(sizeof(*app->scene));
    app->pixels = (Dk1Rgba8 *)calloc(
        (size_t)app->width * (size_t)app->height, sizeof(*app->pixels));
    app->dib_pixels = (uint32_t *)calloc(
        (size_t)app->width * (size_t)app->height, sizeof(*app->dib_pixels));
    if (app->scene == NULL || app->pixels == NULL || app->dib_pixels == NULL)
        return false;
    if (!dk1_rom_load_file(rom_path, &app->owned, &app->rom) ||
        !dk1_rom_identity(&app->rom, &identity) ||
        !dk1_rom_identity_is_supported_rev2(&identity) ||
        !dk1_scene_memory_load(&app->rom, app->level, false, false,
                               app->scene) ||
        !dk1_level_software_frontend_init(
            &app->rom, app->scene, app->level,
            (uint16_t)app->width, (uint16_t)app->height,
            &app->frontend, &app->source_stats))
        return false;

    memset(&app->bitmap_info, 0, sizeof(app->bitmap_info));
    app->bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    app->bitmap_info.bmiHeader.biWidth = app->width;
    app->bitmap_info.bmiHeader.biHeight = -app->height;
    app->bitmap_info.bmiHeader.biPlanes = 1;
    app->bitmap_info.bmiHeader.biBitCount = 32;
    app->bitmap_info.bmiHeader.biCompression = BI_RGB;
    app->ready = true;
    return true;
}

static void cleanup_game(Dk1Win32App *app) {
    if (app == NULL) return;
    free(app->dib_pixels);
    free(app->pixels);
    free(app->scene);
    dk1_rom_free(&app->owned);
    app->dib_pixels = NULL;
    app->pixels = NULL;
    app->scene = NULL;
}

static bool create_window(Dk1Win32App *app, HINSTANCE instance) {
    WNDCLASSA window_class;
    RECT rectangle;
    memset(&window_class, 0, sizeof(window_class));
    window_class.lpfnWndProc = window_proc;
    window_class.hInstance = instance;
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    window_class.lpszClassName = "Dk1FirstLevelPreview";
    if (!RegisterClassA(&window_class) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
        return false;

    rectangle.left = 0;
    rectangle.top = 0;
    rectangle.right = app->width * 2;
    rectangle.bottom = app->height * 2;
    AdjustWindowRect(&rectangle, WS_OVERLAPPEDWINDOW, FALSE);
    app->window = CreateWindowExA(
        0, window_class.lpszClassName,
        "DK1 Jungle Hijinxs provisional preview",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rectangle.right - rectangle.left,
        rectangle.bottom - rectangle.top,
        NULL, NULL, instance, NULL);
    return app->window != NULL;
}

static void update_title(Dk1Win32App *app) {
    char title[256];
    if (app == NULL || app->window == NULL) return;
    snprintf(title, sizeof(title),
             "DK1 Jungle Hijinxs provisional preview - X:%u - "
             "Arrows/A,D move | Z jump | U barrel | Esc exit",
             (unsigned)app->frontend.player_preview.motion.world_x);
    SetWindowTextA(app->window, title);
}

static bool reached_provisional_exit(const Dk1Win32App *app) {
    uint32_t exit_x;
    if (app == NULL || app->scene == NULL) return false;
    exit_x = (uint32_t)app->scene->camera.maximum_x +
             (uint32_t)app->width - 64u;
    return (uint32_t)app->frontend.player_preview.motion.world_x >= exit_x;
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous_instance,
                   LPSTR command_line, int show_command) {
    Dk1Win32App app;
    Dk1RgbaSurface surface;
    MSG message;
    char rom_path[MAX_PATH];
    ULONGLONG next_frame;
    ULONGLONG title_time;
    int result = 1;
    (void)previous_instance;
    (void)command_line;
    (void)show_command;

    memset(&app, 0, sizeof(app));
    app.level = DK1_DEMO_LEVEL;
    app.width = DK1_DEMO_WIDTH;
    app.height = DK1_DEMO_HEIGHT;
    app.running = true;
    g_app = &app;

    if (__argc >= 2) {
        strncpy(rom_path, __argv[1], MAX_PATH - 1u);
        rom_path[MAX_PATH - 1u] = '\0';
    } else if (!choose_rom(rom_path)) {
        g_app = NULL;
        return 0;
    }
    if (__argc >= 3) {
        unsigned long parsed = strtoul(__argv[2], NULL, 0);
        if (parsed >= DK1_SCENE_LEVEL_COUNT) {
            MessageBoxA(NULL, "Invalid level ID.", "DK1 preview", MB_ICONERROR);
            g_app = NULL;
            return 2;
        }
        app.level = (uint16_t)parsed;
    }

    if (!initialize_game(&app, rom_path)) {
        MessageBoxA(NULL,
            "Could not initialize the preview. Select an unheadered USA Rev 2 ROM.",
            "DK1 preview", MB_ICONERROR);
        goto cleanup;
    }
    if (!create_window(&app, instance)) {
        MessageBoxA(NULL, "Could not create the Windows preview window.",
                    "DK1 preview", MB_ICONERROR);
        goto cleanup;
    }

    surface = (Dk1RgbaSurface){app.pixels, (size_t)app.width,
                               (size_t)app.height, (size_t)app.width};
    next_frame = GetTickCount64();
    title_time = next_frame;
    while (app.running) {
        while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) {
            if (message.message == WM_QUIT) app.running = false;
            TranslateMessage(&message);
            DispatchMessageA(&message);
        }
        if (!app.running) break;
        if (!dk1_software_frontend_step(app.scene, app.held, &app.frontend) ||
            !dk1_software_frontend_render(&app.rom, app.scene,
                                          &app.frontend, surface)) {
            MessageBoxA(app.window,
                        "The provisional runtime stopped because a frame failed.",
                        "DK1 preview", MB_ICONERROR);
            break;
        }
        convert_frame(&app);
        InvalidateRect(app.window, NULL, FALSE);

        if (!app.completed && app.level == DK1_DEMO_LEVEL &&
            reached_provisional_exit(&app)) {
            app.completed = true;
            MessageBoxA(app.window,
                "Provisional Jungle Hijinxs exit reached.\n\n"
                "This proves the current terrain, camera and player route can be "
                "traversed; collectibles, all actors, audio and the authentic "
                "exit sequence are still being connected.",
                "First-level preview complete", MB_ICONINFORMATION);
            app.running = false;
        }

        if (GetTickCount64() - title_time >= 500u) {
            update_title(&app);
            title_time = GetTickCount64();
        }
        next_frame += 16u;
        if (next_frame > GetTickCount64())
            Sleep((DWORD)(next_frame - GetTickCount64()));
        else
            next_frame = GetTickCount64();
    }
    result = 0;

cleanup:
    if (app.window != NULL && IsWindow(app.window)) DestroyWindow(app.window);
    cleanup_game(&app);
    g_app = NULL;
    return result;
}
